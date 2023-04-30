#pragma once

#include <sp/StringView.h>

typedef struct {
    StringView mView;
    size_t mPos;
} StringRange;

static inline StringRange StringRange_create(const char *s, size_t len) {
    return (StringRange){
            .mView = (StringView){.s = s, .len = len},
            .mPos = 0,
    };
}
static inline char StringRange_next(StringRange *self) {
    if (self->mPos >= self->mView.len) {
        return '\0';
    }

    return self->mView.s[self->mPos++];
}

// Will read until a \n; result will not include the \n
static inline bool StringRange_nextLine(StringRange *self, StringView *result) {
    if (self->mPos >= self->mView.len) {
        return false;
    }

    const char *found = self->mView.s + self->mView.len;
    const StringView line = SubString(self->mView, self->mPos);
    sv_for_each(line, it) {
        if (*it == '\n') {
            found = it;
            break;
        }
    }

    self->mPos = found - self->mView.s + 1;
    *result = (StringView){.s = line.s, .len = (size_t)(found - line.s)};
    return true;
}
