#pragma once

#include <Common.h>
#include <string.h>  // memset

typedef struct {
    // 63 characters, 1 reserved 0
    char buf[64];
    size_t len;
} TypingBuffer;

inline void TypingBuffer_Init(TypingBuffer *buf) {
    memset(buf->buf, 0, sizeof(buf->buf));
    buf->len = 0;
}

inline bool TypingBuffer_IsExhausted(const TypingBuffer *buf) {
    return buf->len >= sizeof(buf->buf) - 1;
}

inline void TypingBuffer_Append(TypingBuffer *buf, char c) {
    if (!TypingBuffer_IsExhausted(buf))
        buf->buf[buf->len++] = c;
}
inline void TypingBuffer_Backspace(TypingBuffer *buf) {
    assert(buf->len < sizeof(buf->buf));
    if (buf->len > 0)
        buf->buf[buf->len--] = '\0';
}