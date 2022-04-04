#pragma once

#include <Common.h>

typedef struct {
    const char *mStr;
    size_t mPos;
    size_t mLen;
} StringRange;

static inline StringRange StringRange_create(const char *s, size_t len) {
    return (StringRange){
        .mStr = s,
        .mPos = 0,
        .mLen = len,
    };
}
static inline char StringRange_next(StringRange *self) {
    if (self->mPos >= self->mLen)
        return '\0';

    return self->mStr[self->mPos++];
}
