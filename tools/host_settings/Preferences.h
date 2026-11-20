#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <variant>

enum PreferenceType { PT_U8, PT_U16, PT_U32, PT_STR, PT_INVALID };
using LegacyValue = std::variant<uint8_t, uint16_t, uint32_t, std::string>;
using LegacyNamespace = std::map<std::string, LegacyValue>;
inline std::map<std::string, LegacyNamespace> legacy;

// The firmware reads old Preferences keys but writes the new state through NVS.
class Preferences {
public:
    bool begin(const char* name, bool readOnly) {
        if (!readOnly) std::abort();
        const auto entry = legacy.find(name);
        if (entry == legacy.end()) return false;
        values = &entry->second;
        return true;
    }

    void end() { values = nullptr; }
    bool isKey(const char* key) const { return find(key) != nullptr; }
    PreferenceType getType(const char* key) const {
        const auto* value = find(key);
        return value ? static_cast<PreferenceType>(value->index()) : PT_INVALID;
    }

    uint32_t getUInt(const char* key, uint32_t fallback = 0) const { return get(key, fallback); }
    uint16_t getUShort(const char* key) const { return get<uint16_t>(key, 0); }
    uint8_t getUChar(const char* key, uint8_t fallback = 0) const { return get(key, fallback); }
    bool getBool(const char* key, bool fallback = false) const { return get<uint8_t>(key, fallback) != 0; }

    size_t getStringLength(const char* key) const {
        const auto* value = std::get_if<std::string>(find(key));
        return value ? value->size() + 1 : 0;
    }

    size_t getString(const char* key, char* out, size_t size) const {
        const size_t length = getStringLength(key);
        if (!length || length > size) return 0;
        memcpy(out, std::get<std::string>(*find(key)).c_str(), length);
        return length;
    }

private:
    const LegacyNamespace* values = nullptr;

    const LegacyValue* find(const char* key) const {
        if (!values) std::abort();
        const auto entry = values->find(key);
        return entry == values->end() ? nullptr : &entry->second;
    }

    template <typename T>
    T get(const char* key, T fallback) const {
        const auto* value = std::get_if<T>(find(key));
        return value ? *value : fallback;
    }
};
