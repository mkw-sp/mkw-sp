#pragma once

#include <sp/StringRange.h>

typedef struct {
    StringRange mString;
    StringView currentSection;
    int sectionLineNum;
    int sectionLineCharacter;
    u32 lineNum;
    int state; // Internal
} IniRange;

typedef struct {
    StringView section;  // Optional
    int sectionLineNum;
    int sectionLineCharacter; // 0 - start of line

    StringView key;
    StringView value;  // Optional
    int keyvalLineNum;
    int keyvalLineCharacter;  // 0 - start of line
} IniProperty;

IniRange IniRange_create(const char *s, size_t len);
bool IniRange_next(IniRange *self, IniProperty *prop);
