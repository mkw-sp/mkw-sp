#include "IniReader.h"

IniRange IniRange_create(const char *s, size_t len) {
    return (IniRange){
        .mString = StringRange_create(s, len),
        .currentSection = StringView_create(""),
        .lineNum = 0,
    };
}

bool IniRange_next(IniRange *self, IniProperty *prop) {
    assert(self != NULL);
    assert(prop != NULL);

    for (;; ++self->lineNum) {
        StringView rawLine;
        if (!StringRange_nextLine(&self->mString, &rawLine)) {
            // End of file
            return false;
        }

        // Trim comments and leading zeroes
        StringView line =
                SplitLeft(SkipLeading(rawLine, ' '), StringView_create("#;"), NULL);
        if (line.len == 0) {
            continue;
        }

        // Section header
        if (line.s[0] == '[') {
            const StringView section =
                    SplitLeft(SubString(line, 1), StringView_create("]"), NULL);
            if (section.len == 0) {
                return false;
            }
            // Ignore everything trailing ']'; effectively create an implicit comment
            self->currentSection = section;
            continue;
        }
        // Key
        if (ContainsChar(line, ']') || ContainsChar(line, '[')) {
            // Invalid
            return false;
        }
        if (line.s[0] == '=') {
            // Invalid
            return false;
        }

        StringView remaining;  // Does not include =
        const StringView key =
                SkipTrailing(SplitLeft(line, StringView_create("="), &remaining), ' ');
        if (remaining.len < 1) {
            // Invalid
            return false;
        }
        const StringView value = SkipLeading(remaining, ' ');
        *prop = (IniProperty){
            .section = self->currentSection,
            .key = key,
            .value = value,
        };
        return true;
    }
}
