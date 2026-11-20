#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "Preferences.h"

using nvs_handle_t = unsigned;
using esp_err_t = int;
enum { ESP_OK, ESP_FAIL, ESP_ERR_NVS_NOT_FOUND, ESP_ERR_NVS_INVALID_LENGTH, ESP_ERR_NVS_TYPE_MISMATCH };
enum nvs_open_mode_t { NVS_READONLY, NVS_READWRITE };

struct PowerLoss {};
enum class WriteFault { None, OpenFailure, SetFailure, BeforeSet, AfterSet, CommitFailure, EraseFailure };
inline std::vector<uint8_t> flashState;
inline WriteFault nextWriteFault = WriteFault::None;

inline esp_err_t nvs_open(const char* name, nvs_open_mode_t mode, nvs_handle_t* out) {
    static const std::string names[] = {"bridge-state", "bridge", "bridge-prev"};
    size_t index = 0;
    constexpr size_t count = sizeof(names) / sizeof(names[0]);
    while (index < count && names[index] != name) ++index;
    if (index == count) std::abort();
    if (mode == NVS_READWRITE && nextWriteFault == WriteFault::OpenFailure) {
        nextWriteFault = WriteFault::None;
        return ESP_FAIL;
    }
    if (index && mode == NVS_READONLY && !legacy.count(name)) return ESP_ERR_NVS_NOT_FOUND;
    *out = 2 * index + mode;
    return ESP_OK;
}

inline esp_err_t nvs_get_blob(nvs_handle_t, const char* key, void* out, size_t* size) {
    if (strcmp(key, "state") != 0) std::abort();
    if (flashState.empty()) return ESP_ERR_NVS_NOT_FOUND;
    if (!out || *size < flashState.size()) {
        *size = flashState.size();
        return out ? ESP_ERR_NVS_INVALID_LENGTH : ESP_OK;
    }
    *size = flashState.size();
    memcpy(out, flashState.data(), *size);
    return ESP_OK;
}

inline esp_err_t nvs_set_blob(nvs_handle_t handle, const char* key, const void* data, size_t size) {
    if (handle != NVS_READWRITE || strcmp(key, "state") != 0) std::abort();
    const WriteFault fault = nextWriteFault;
    if (fault != WriteFault::CommitFailure && fault != WriteFault::EraseFailure) nextWriteFault = WriteFault::None;
    if (fault == WriteFault::SetFailure) return ESP_FAIL;
    if (fault == WriteFault::BeforeSet) throw PowerLoss{};
    // Model one whole-record replacement. NVS changes may become durable before commit.
    const auto* bytes = static_cast<const uint8_t*>(data);
    flashState.assign(bytes, bytes + size);
    if (fault == WriteFault::AfterSet) throw PowerLoss{};
    return ESP_OK;
}

inline esp_err_t nvs_commit(nvs_handle_t handle) {
    if (handle % 2 != NVS_READWRITE) std::abort();
    if (nextWriteFault == WriteFault::CommitFailure) {
        nextWriteFault = WriteFault::None;
        return ESP_FAIL;
    }
    return ESP_OK;
}

inline esp_err_t nvs_erase_all(nvs_handle_t handle) {
    if (handle == NVS_READWRITE || handle % 2 != NVS_READWRITE) std::abort();
    if (nextWriteFault == WriteFault::EraseFailure) {
        nextWriteFault = WriteFault::None;
        return ESP_FAIL;
    }
    legacy.erase(handle / 2 == 1 ? "bridge" : "bridge-prev");
    return ESP_OK;
}

inline void nvs_close(nvs_handle_t) {}
