#pragma once

#include <sp/StringRange.h>

typedef struct {
    StringRange mString;
    StringView currentSection;
    u32 lineNum;
    int state; // Internal
} IniRange;

typedef struct {
    StringView section;  // Optional
    StringView key;
    StringView value;  // Optional
} IniProperty;

IniRange IniRange_create(const char *s, size_t len);
bool IniRange_next(IniRange *self, IniProperty *prop);
