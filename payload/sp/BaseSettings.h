#pragma once

#include <Common.h>

typedef struct {
    const char *name;
    u32 category;
    u32 defaultValue;

    const char **enumValues;
    u32 numEnumValues;
} Setting;

typedef struct {
    size_t numValues;
    const Setting* fieldDescriptors;

    const char **categoryNames;
} BaseSettingsDescriptor;

void SpSetting_ParseFromIni(
        const char *str, size_t len, const BaseSettingsDescriptor *desc, u32* valueArray);
void SpSetting_WriteToIni(
        char *buf, size_t maxlen, const BaseSettingsDescriptor *desc, const u32 *valueArray);
void SpSetting_ResetToDefault(const BaseSettingsDescriptor *desc, u32 *valueArray);

// Ignores sections
void SpSetting_Set(const BaseSettingsDescriptor *desc, u32 *valueArray, const char *key,
        const char *value);
