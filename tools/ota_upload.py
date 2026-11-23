"""Upload a firmware image to the bridge over Wi-Fi with the stored OTA password.

BOUNDARY: the owner starts this script. An AI assistant may prepare the image
and the command, but never runs it, never reads, prints or copies the stored
password and never passes it to a command.

The OTA password lives in the Windows Credential Manager (generic credential,
target MomoRC/BridgeTom-OTA). The script loads espota.py from the newest
installed esp32 board package and calls it in-process, so the password never
appears on a command line.

    python tools/ota_upload.py build/bridge/bridge.ino.bin            # hotspot 10.10.10.10
    python tools/ota_upload.py build/bridge/bridge.ino.bin 192.0.2.5
    python tools/ota_upload.py --store     # enter or replace the password, then exit
    python tools/ota_upload.py --forget    # delete the stored password

Windows only, standard library only.
"""

import argparse
import ctypes
import getpass
import importlib.util
import logging
import os
import random
import re
import sys
from ctypes import wintypes

TARGET = "MomoRC/BridgeTom-OTA"
DEFAULT_HOST = "10.10.10.10"
OTA_PORT = 3232
ESP32_HW = os.path.join(os.environ.get("LOCALAPPDATA", ""), "Arduino15", "packages", "esp32", "hardware", "esp32")

# --- Windows Credential Manager (advapi32) ---------------------------------

CRED_TYPE_GENERIC = 1
CRED_PERSIST_LOCAL_MACHINE = 2
ERROR_NOT_FOUND = 1168


class _FILETIME(ctypes.Structure):
    _fields_ = [("dwLowDateTime", wintypes.DWORD), ("dwHighDateTime", wintypes.DWORD)]


class _CREDENTIALW(ctypes.Structure):
    _fields_ = [
        ("Flags", wintypes.DWORD),
        ("Type", wintypes.DWORD),
        ("TargetName", wintypes.LPWSTR),
        ("Comment", wintypes.LPWSTR),
        ("LastWritten", _FILETIME),
        ("CredentialBlobSize", wintypes.DWORD),
        ("CredentialBlob", ctypes.POINTER(ctypes.c_ubyte)),
        ("Persist", wintypes.DWORD),
        ("AttributeCount", wintypes.DWORD),
        ("Attributes", ctypes.c_void_p),
        ("TargetAlias", wintypes.LPWSTR),
        ("UserName", wintypes.LPWSTR),
    ]


_PCRED = ctypes.POINTER(_CREDENTIALW)


def _advapi():
    dll = ctypes.WinDLL("advapi32", use_last_error=True)
    dll.CredWriteW.argtypes = [_PCRED, wintypes.DWORD]
    dll.CredWriteW.restype = wintypes.BOOL
    dll.CredReadW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD, ctypes.POINTER(_PCRED)]
    dll.CredReadW.restype = wintypes.BOOL
    dll.CredDeleteW.argtypes = [wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD]
    dll.CredDeleteW.restype = wintypes.BOOL
    dll.CredFree.argtypes = [ctypes.c_void_p]
    dll.CredFree.restype = None
    return dll


def cred_write(target, secret):
    blob = secret.encode("utf-16-le")
    buf = (ctypes.c_ubyte * len(blob)).from_buffer_copy(blob)
    cred = _CREDENTIALW()
    cred.Type = CRED_TYPE_GENERIC
    cred.TargetName = target
    cred.CredentialBlobSize = len(blob)
    cred.CredentialBlob = ctypes.cast(buf, ctypes.POINTER(ctypes.c_ubyte))
    cred.Persist = CRED_PERSIST_LOCAL_MACHINE
    cred.UserName = "owner"
    try:
        if not _advapi().CredWriteW(ctypes.byref(cred), 0):
            raise ctypes.WinError(ctypes.get_last_error())
    finally:
        ctypes.memset(buf, 0, len(blob))


def cred_read(target):
    """Stored secret or None. Used in-process only; never printed."""
    dll = _advapi()
    pcred = _PCRED()
    if not dll.CredReadW(target, CRED_TYPE_GENERIC, 0, ctypes.byref(pcred)):
        err = ctypes.get_last_error()
        if err == ERROR_NOT_FOUND:
            return None
        raise ctypes.WinError(err)
    try:
        cred = pcred.contents
        return ctypes.string_at(cred.CredentialBlob, cred.CredentialBlobSize).decode("utf-16-le")
    finally:
        dll.CredFree(pcred)


def cred_delete(target):
    """Delete the entry; False if there was none."""
    if _advapi().CredDeleteW(target, CRED_TYPE_GENERIC, 0):
        return True
    err = ctypes.get_last_error()
    if err == ERROR_NOT_FOUND:
        return False
    raise ctypes.WinError(err)


def ask_and_store(target):
    first = getpass.getpass(f"OTA password for {target}: ")
    if not first:
        print("Empty input, nothing stored.")
        return False
    if getpass.getpass("Repeat: ") != first:
        print("The entries differ, nothing stored.")
        return False
    cred_write(target, first)
    print(f"Stored in Windows Credential Manager as {target}.")
    return True


def get_password(target):
    secret = cred_read(target)
    if secret is not None:
        return secret
    secret = getpass.getpass(f"No OTA password stored as {target}. OTA password: ")
    if secret and input("Store it in Windows Credential Manager? [y/N] ").strip().lower() == "y":
        cred_write(target, secret)
        print(f"Stored as {target}.")
    return secret


# --- espota.py ---------------------------------------------------------------


def _version_key(name):
    return [int(p) if p.isdigit() else 0 for p in re.split(r"[.\-]", name)]


def find_espota():
    """espota.py of the newest installed esp32 board package, or None."""
    if not os.path.isdir(ESP32_HW):
        return None
    versions = sorted(os.listdir(ESP32_HW), key=_version_key, reverse=True)
    for version in versions:
        path = os.path.join(ESP32_HW, version, "tools", "espota.py")
        if os.path.isfile(path):
            return path
    return None


def load_espota(path):
    spec = importlib.util.spec_from_file_location("espota", path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def upload(espota, host, password, image, timeout):
    """Call espota.serve() in-process (main() would log the options, password included, at debug level)."""
    logging.basicConfig(level=logging.WARNING, format="%(asctime)-8s [%(levelname)s]: %(message)s", datefmt="%H:%M:%S")
    espota.PROGRESS = True
    espota.TIMEOUT = timeout
    return espota.serve(
        host, "0.0.0.0", OTA_PORT, random.randint(10000, 60000), password, False, image, espota.FLASH
    )


def main():
    parser = argparse.ArgumentParser(
        description="Upload a firmware image to the bridge with the OTA password from the Windows Credential Manager. "
        "The owner starts this script; assistants only prepare the image and the command."
    )
    parser.add_argument("image", nargs="?", help="Firmware image (.bin), for example build/bridge/bridge.ino.bin")
    parser.add_argument("host", nargs="?", default=DEFAULT_HOST, help=f"Bridge address (default {DEFAULT_HOST}, the hotspot)")
    parser.add_argument("--target", default=TARGET, help=f"Credential Manager target (default {TARGET})")
    parser.add_argument("--store", action="store_true", help="Enter or replace the stored OTA password, then exit")
    parser.add_argument("--forget", action="store_true", help="Delete the stored OTA password, then exit")
    parser.add_argument("--espota", help="Path to espota.py (default: newest installed esp32 board package)")
    parser.add_argument("--timeout", type=int, default=10, help="Seconds to wait for the bridge to accept (default 10)")
    args = parser.parse_args()

    if args.store:
        return 0 if ask_and_store(args.target) else 1
    if args.forget:
        print("Deleted." if cred_delete(args.target) else "Nothing stored under that name.")
        return 0
    if not args.image:
        parser.error("image is required (or use --store / --forget)")
    if not os.path.isfile(args.image):
        print(f"Image not found: {args.image}")
        return 1

    espota_path = args.espota or find_espota()
    if not espota_path or not os.path.isfile(espota_path):
        print(f"espota.py not found under {ESP32_HW}; install the esp32 board package or pass --espota.")
        return 1

    password = get_password(args.target)
    if not password:
        print("No OTA password, stopped.")
        return 1

    print(f"Uploading {args.image} to {args.host} with {espota_path}")
    try:
        result = upload(load_espota(espota_path), args.host, password, args.image, args.timeout)
    except Exception as exc:  # report without a traceback that could carry local variables
        print(f"Error: {type(exc).__name__}: {exc}")
        result = 1
    finally:
        password = None
    print("UPLOAD OK" if result == 0 else "UPLOAD FAILED")
    return 0 if result == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
