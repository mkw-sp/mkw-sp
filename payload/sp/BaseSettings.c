#include "BaseSettings.h"
#include <revolution.h>
#include <stdarg.h>
#include <stdio.h>
#include "IniReader.h"

// Ignores sections
void SpSetting_Set(const BaseSettingsDescriptor *desc, u32 *valueArray, const char *key,
        const char *value) {
    int setting = -1;
    for (size_t i = 0; i < desc->numValues; ++i) {
        if (!strcmp(key, desc->fieldDescriptors[i].name)) {
            setting = i;
            break;
        }
    }
    if (setting == -1) {
        SP_LOG("Unknown key *::%s", key);
        SP_LOG("Expected one of:");
        char validKeys[256];
        size_t cursor = 0;

        for (size_t i = 0; i < desc->numValues; ++i) {
            if (cursor >= sizeof(validKeys) - 3) {
                break;
            }
            cursor += snprintf(validKeys + cursor, sizeof(validKeys) - cursor, "%s ",
                    desc->fieldDescriptors[i].name);
        }
        SP_LOG("%s", validKeys);
        return;
    }
    const Setting *spSetting = &desc->fieldDescriptors[setting];

    int valueIt = -1;
    for (size_t i = 0; i < spSetting->numEnumValues; ++i) {
        if (!strcmp(spSetting->enumValues[i], value)) {
            valueIt = i;
            break;
        }
    }

    if (valueIt == -1) {
        SP_LOG("Unknown value \"%s\" for %s::%s", value,
                desc->categoryNames[spSetting->category], spSetting->name);
        SP_LOG("Expected one of:");
        for (size_t i = 0; i < spSetting->numEnumValues; ++i) {
            SP_LOG("- %s (%u)", spSetting->enumValues[i], (unsigned)i);
        }
        return;
    }
    SP_LOG("Setting %s::%s to %s (%i)", desc->categoryNames[spSetting->category],
            spSetting->name, spSetting->enumValues[valueIt], valueIt);
    valueArray[setting] = valueIt;
}

void SpSetting_ParseFromIni(const char *str, size_t len,
        const BaseSettingsDescriptor *desc, u32 *valueArray) {
    IniRange iniRange = IniRange_create(str, len);

    IniProperty prop;
    while (IniRange_next(&iniRange, &prop)) {
        int setting = -1;
        for (size_t i = 0; i < desc->numValues; ++i) {
            const Setting *fieldDescriptor = &desc->fieldDescriptors[i];

            const char *categoryName = desc->categoryNames[fieldDescriptor->category];
            if (!StringView_equalsCStr(prop.section, categoryName)) {
                continue;
            }
            if (!StringView_equalsCStr(prop.key, fieldDescriptor->name)) {
                continue;
            }

            setting = i;
            break;
        }
        if (setting == -1) {
            const char *section_cstr = sv_as_cstr(prop.section, 64);
            const char *key_cstr = sv_as_cstr(prop.key, 64);
            SP_LOG("Unknown key %s::%s", section_cstr, key_cstr);
            continue;
        }
        const Setting *spSetting = &desc->fieldDescriptors[setting];

        int value = -1;
        for (size_t i = 0; i < spSetting->numEnumValues; ++i) {
            if (StringView_equalsCStr(prop.value, spSetting->enumValues[i])) {
                value = i;
                break;
            }
        }

        if (value == -1) {
            const char *value_cstr = sv_as_cstr(prop.value, 64);
            SP_LOG("Unknown value \"%s\" for %s::%s", value_cstr,
                    desc->categoryNames[spSetting->category], spSetting->name);
            SP_LOG("Expected one of:");
            for (size_t i = 0; i < spSetting->numEnumValues; ++i) {
                SP_LOG("- %s (%u)", spSetting->enumValues[i], (unsigned)i);
            }
            continue;
        }

        SP_LOG("Setting %s to %s (%i)", spSetting->name, spSetting->enumValues[value],
                value);
        valueArray[setting] = value;
    }
}
static void Print(char **buf, size_t *maxlen, const char *s, ...) {
    if (*maxlen == 0) {
        return;
    }

    va_list l;
    va_start(l, s);
    int written = vsnprintf(*buf, *maxlen, s, l);
    va_end(l);
    assert(written >= 0);
    *buf += written;
    *maxlen -= written;
}
void SpSetting_WriteToIni(char *buf, size_t maxlen, const BaseSettingsDescriptor *desc,
        const u32 *valueArray) {
    u32 lastCategory = ~0;
    for (size_t i = 0; i < desc->numValues; ++i) {
        const Setting *spSetting = &desc->fieldDescriptors[i];

        if (lastCategory != spSetting->category) {
            Print(&buf, &maxlen, "\n[%s]\n", desc->categoryNames[spSetting->category]);
            lastCategory = spSetting->category;
        }
        Print(&buf, &maxlen, "%s = %s\n", spSetting->name,
                spSetting->enumValues[valueArray[i]]);
    }
}
void SpSetting_ResetToDefault(const BaseSettingsDescriptor *desc, u32 *valueArray) {
    for (size_t i = 0; i < desc->numValues; ++i) {
        valueArray[i] = desc->fieldDescriptors[i].defaultValue;
    }
}
