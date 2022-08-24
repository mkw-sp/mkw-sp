#pragma once

#include "sp/settings/IniReader.hh"

extern "C" {
#include <revolution.h>
}
#include <vendor/magic_enum/magic_enum.hpp>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace SP::Settings {

template <typename C>
struct Entry {
    C category;
    std::string_view name;
    u32 messageId;
    u32 defaultValue;
    u32 valueCount;
    u32 valueOffset = 0;
    const std::string_view *valueNames;
    const u32 *valueMessageIds;
    const u32 *valueExplanationMessageIds;
    std::optional<u32> vanillaValue{};
    bool hidden = false;
};

template <typename C>
struct Group {
    const char *name;
    const std::string_view *categoryNames;
    const u32 *categoryMessageIds;
    u32 entryCount;
    const Entry<C> *entries;
};

template <typename S, S T>
struct Helper;

template <typename C, Group<C> G>
class Settings {
public:
    void reset() {
        for (u32 i = 0; i < G.entryCount; ++i) {
            m_values[i] = G.entries[i].defaultValue;
        }
    }

    void readIni(const char *ini, size_t length) {
        std::string_view view(ini, length);
        IniReader reader(view);
        std::optional<IniReader::Property> property;
        while ((property = reader.next())) {
            std::string_view section = property->section;
            std::string_view key = property->key;
            std::string_view value = property->value;
            set(section, key, value, false);
        }
    }

    void writeIni(char *ini, size_t length) {
        assert(ini);

        Print(ini, length, "# %s\n", G.name);

        u32 lastCategory = magic_enum::enum_count<C>();
        for (u32 i = 0; i < G.entryCount; ++i) {
            const auto &entry = G.entries[i];
            auto entryName = entry.name;

            if (lastCategory != static_cast<u32>(entry.category)) {
                auto categoryName = G.categoryNames[static_cast<u32>(entry.category)];
                Print(ini, length, "\n[%.*s]\n", categoryName.length(), categoryName.data());
                lastCategory = static_cast<u32>(entry.category);
            }

            // 0 -> hex
            if (entry.valueCount == 0) {
                Print(ini, length, "%.*s = %08X\n", entryName.length(), entryName.data(),
                        m_values[i]);
                continue;
            }

            // Numeric value
            if (!entry.valueNames) {
                Print(ini, length, "%.*s = %u\n", entryName.length(), entryName.data(),
                        m_values[i]);
                continue;
            }

            auto valueName = entry.valueNames[m_values[i]];
            Print(ini, length, "%.*s = %.*s\n", entryName.length(), entryName.data(),
                    valueName.length(), valueName.data());
        }
    }

    u32 get(u32 setting) const {
        return m_values[setting];
    }

    void set(u32 setting, u32 value) {
        m_values[setting] = value;
    }

    template <typename S, S T>
    Helper<S, T>::type get() const {
        return static_cast<Helper<S, T>::type>(m_values[static_cast<u32>(T)]);
    }

    template <typename S, S T>
    void set(Helper<S, T>::type value) {
        m_values[static_cast<u32>(T)] = static_cast<u32>(value);
    }

    void set(const char *key, const char *value) {
        set(std::string_view(), std::string_view(key), std::string_view(value), true);
    }

private:
    void set(std::string_view section, std::string_view key, std::string_view value, bool verbose) {
        std::optional<u32> setting{};
        for (u32 i = 0; i < G.entryCount; ++i) {
            const auto &entry = G.entries[i];

            if (section.data() && section != G.categoryNames[static_cast<u32>(entry.category)]) {
                continue;
            }
            if (key != entry.name) {
                continue;
            }

            setting = i;
            break;
        }
        if (!setting) {
            if (section.data()) {
                SP_LOG("Unknown key %.*s::%.*s", section.length(), section.data(), key.length(),
                        key.data());
            } else {
                SP_LOG("Unknown key *::%.*s", key.length(), key.data());
            }
            if (verbose) {
                SP_LOG("Expected one of:");
                for (u32 i = 0; i < G.entryCount; i++) {
                    SP_LOG("%.*s", G.entries[i].name.length(), G.entries[i].name.data());
                }
            }
            return;
        }
        const auto &entry = G.entries[*setting];
        auto categoryName = G.categoryNames[static_cast<u32>(entry.category)];

        // Default to hex
        if (entry.valueCount == 0) {
            char tmp[9];
            snprintf(tmp, sizeof(tmp), "%.*s", value.length(), value.data());
            char *end;
            u32 v = strtoul(tmp, &end, 16);
            if (*end != '\0') {
                SP_LOG("Invalid value \"%.*s\" for %.*s::%.*s", value.length(), value.data(),
                        categoryName.length(), categoryName.data(), entry.name.length(),
                        entry.name.data());
                return;
            }
            SP_LOG("Setting %.*s::%.*s to %08x (%i)", categoryName.length(), categoryName.data(),
                    entry.name.length(), entry.name.data(), v, v);
            m_values[*setting] = v;
            return;
        }

        // Numeric value
        if (!entry.valueNames) {
            u32 valueOffset = entry.valueOffset;
            u32 valueCount = entry.valueCount;
            char tmp[9];
            snprintf(tmp, sizeof(tmp), "%.*s", value.length(), value.data());
            char *end;
            u32 v = strtoul(tmp, &end, 10);
            if (*end != '\0' || v < valueOffset || v >= valueOffset + valueCount) {
                SP_LOG("Invalid value \"%.*s\" for %.*s::%.*s", value.length(), value.data(),
                        categoryName.length(), categoryName.data(), entry.name.length(),
                        entry.name.data());
                return;
            }
            SP_LOG("Setting %.*s::%.*s to %u (%i)", categoryName.length(), categoryName.data(),
                    entry.name.length(), entry.name.data(), v, v);
            m_values[*setting] = v;
            return;
        }

        std::optional<u32> v;
        for (u32 i = 0; i < entry.valueCount; ++i) {
            if (entry.valueNames[i] == value) {
                v = i;
                break;
            }
        }
        if (!v) {
            SP_LOG("Unknown value \"%.*s\" for %.*s::%.*s", value.length(), value.data(),
                    categoryName.length(), categoryName.data(), entry.name.length(),
                    entry.name.data());
            SP_LOG("Expected one of:");
            for (u32 i = 0; i < entry.valueCount; ++i) {
                auto valueName = entry.valueNames[i];
                SP_LOG("- %.*s (%u)", valueName.length(), valueName.data(), i);
            }
            return;
        }
        auto valueName = entry.valueNames[*v];
        SP_LOG("Setting %.*s::%.*s to %.*s (%i)", categoryName.length(), categoryName.data(),
                entry.name.length(), entry.name.data(), valueName.length(), valueName.data(), *v);
        m_values[*setting] = *v;
    }

    static void Print(char *&ini, size_t &length, const char *format, ...) {
        va_list list;
        va_start(list, format);
        s32 written = vsnprintf(ini, length, format, list);
        va_end(list);
        assert(written >= 0 && static_cast<size_t>(written) < length);
        ini += written;
        length -= written;
    }

    u32 m_values[G.entryCount];
};

} // namespace SP::Settings
