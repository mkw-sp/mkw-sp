#include "IniReader.h"
#include <revolution.h>
#include <stdarg.h>
#include <stdio.h>

enum {
    STATE_NONE,
    STATE_WANT_SECTION,
    STATE_WANT_SECTION_END,
    STATE_IN_COMMENT,
    STATE_WANT_DELIM,
    STATE_WANT_VALUE,
    STATE_WANT_VALUE_END,
    STATE_IN_COMMENT_WANT_VALUE,
    STATE_POST_WANT_VALUE,
};

IniRange IniRange_create(const char *s, size_t len) {
    return (IniRange){
        .mString = StringRange_create(s, len),
        .currentSection = StringView_create(""),
        .lineNum = 0,
        .sectionLineNum = -1,
        .sectionLineCharacter = -1,
        .state = STATE_NONE,
    };
}
static bool IniRange_WantsValueEnd(int state) {
    switch (state) {
    case STATE_WANT_VALUE_END:
    case STATE_WANT_VALUE /* Empty value */:
    case STATE_IN_COMMENT_WANT_VALUE:
        return true;
    }

    return false;
}
bool IniRange_next(IniRange *self, IniProperty *prop) {
    assert(self != NULL);
    assert(prop != NULL);

    const char *lineStart = self->mString.mView.s + self->mString.mPos;
    const char *token = NULL;
    const char *lastNonSpace = NULL;

    StringView key = StringView_create("");

    const char *const viewEnd = self->mString.mView.s + self->mString.mView.len;
    for (const char *it = lineStart; it != viewEnd; ++it, ++self->mString.mPos) {
        if (self->state == STATE_POST_WANT_VALUE) {
            ++self->lineNum;
            self->state = STATE_NONE;
        }
        switch (*it) {
        case ' ':
            if (self->state == STATE_NONE || self->state == STATE_WANT_DELIM ||
                    self->state == STATE_WANT_VALUE) {
                break;
            }
            // Treat as identifier
            goto Identifier;
        case '[':
            if (self->state == STATE_NONE) {
                token = it;
                lastNonSpace = token;
                self->state = STATE_WANT_SECTION;
                break;
            }
            // Treat as identifier
            goto Identifier;
        case ']':
            if (self->state == STATE_WANT_SECTION_END ||
                    self->state == STATE_WANT_SECTION /* Empty section tag */) {
                self->currentSection = (StringView){
                    .s = token,
                    .len = self->state == STATE_WANT_SECTION ? 0 : it - token,
                };
                self->sectionLineNum = self->lineNum;
                self->sectionLineCharacter = token - lineStart;
                self->state = STATE_NONE;
                break;
            }
            // Treat as identifier
            goto Identifier;
        case '\n':
            if (self->state == STATE_NONE || self->state == STATE_IN_COMMENT) {
                self->state = STATE_NONE;
                ++self->lineNum;
                lineStart = it + 1;
                break;
            }
            if (IniRange_WantsValueEnd(self->state)) {
                ++self->mString.mPos;
            StateWantValue:;
                const StringView value = (StringView){
                    .s = token,
                    .len = self->state == STATE_WANT_VALUE ? 0 : lastNonSpace + 1 - token,
                };
                self->state = STATE_POST_WANT_VALUE;
                *prop = (IniProperty){
                    .section = self->currentSection,
                    .sectionLineNum = self->sectionLineNum,
                    .sectionLineCharacter = self->sectionLineCharacter,
                    .key = key,
                    .value = value,
                    .keyvalLineNum = self->lineNum,
                    .keyvalLineCharacter = key.s - lineStart,
                };
                return true;
            }
            switch (self->state) {
            case STATE_WANT_SECTION:
            case STATE_WANT_SECTION_END:
                SP_LOG("<%i:%i> Expecting ']', instead saw end of line", self->lineNum,
                        it - lineStart);
                return false;
            case STATE_WANT_DELIM:
                SP_LOG("<%i:%i> Expecting '=' or ':', instead saw end of line",
                        self->lineNum, it - lineStart);
                return false;
            }

            __builtin_unreachable();
        case '#':
        case ';':
            if (self->state == STATE_NONE || self->state == STATE_IN_COMMENT) {
                self->state = STATE_IN_COMMENT;
                break;
            }
            if (IniRange_WantsValueEnd(self->state)) {
                self->state = STATE_IN_COMMENT_WANT_VALUE;
                break;
            }
            switch (self->state) {
            case STATE_WANT_SECTION:
            case STATE_WANT_SECTION_END:
                SP_LOG("<%i:%i> Expecting ']', instead saw comment", self->lineNum,
                        it - lineStart);
                return false;
            case STATE_WANT_DELIM:
                SP_LOG("<%i:%i> Expecting '=' or ':', instead saw comment", self->lineNum,
                        it - lineStart);
                return false;
            }

            __builtin_unreachable();
        case '=':
        case ':':
            if (self->state == STATE_WANT_DELIM) {
                self->state = STATE_WANT_VALUE;
                key = (StringView){
                    .s = token,
                    .len = lastNonSpace + 1 - token,
                };
                break;
            }
            // Treat as identifier
            goto Identifier;
        default:
            if (self->state == STATE_NONE) {
                self->state = STATE_WANT_DELIM;
                token = it;
                lastNonSpace = token;
                break;
            }
            // Treat as identifier
        Identifier:
            switch (self->state) {
            case STATE_IN_COMMENT:
            case STATE_IN_COMMENT_WANT_VALUE:
                continue;
            case STATE_WANT_SECTION:
                token = it;
                lastNonSpace = token;
                self->state = STATE_WANT_SECTION_END;
                continue;
            case STATE_WANT_VALUE:
                token = it;
                lastNonSpace = token;
                self->state = STATE_WANT_VALUE_END;
                continue;
            case STATE_WANT_SECTION_END:
            case STATE_WANT_DELIM:
            case STATE_WANT_VALUE_END:
                if (*it != ' ') {
                    lastNonSpace = it;
                }
                continue;
            case STATE_NONE:
            case STATE_POST_WANT_VALUE:
                __builtin_unreachable();
            }

            __builtin_unreachable();
        }
    }
    switch (self->state) {
    case STATE_NONE:
    case STATE_IN_COMMENT:
        return false;
    case STATE_WANT_SECTION:
    case STATE_WANT_SECTION_END:
        SP_LOG("<%i:%i> Expecting ']', instead saw EOF", self->lineNum,
                viewEnd - lineStart);
        return false;
    case STATE_WANT_DELIM:
        SP_LOG("<%i:%i> Expecting '=' or ':', instead saw EOF", self->lineNum,
                viewEnd - lineStart);
        return false;
    case STATE_WANT_VALUE:
    case STATE_WANT_VALUE_END:
    case STATE_IN_COMMENT_WANT_VALUE:
        goto StateWantValue;
    case STATE_POST_WANT_VALUE:
        return false;
    }

    __builtin_unreachable();
    return false;
}
