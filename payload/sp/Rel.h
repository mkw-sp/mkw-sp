#pragma once

#include <Common.h>

extern u8 rel_start[];
extern u8 rel_end[];
extern u8 rel_text_start[];
extern u8 rel_text_end[];
extern u8 rel_rodata_end[];

static inline void *Rel_getStart(void) {
    return rel_start;
}

static inline void *Rel_getEnd(void) {
    return rel_end;
}

static inline u32 Rel_getSize(void) {
    return rel_end - rel_start;
}

static inline void *Rel_getTextSectionStart(void) {
    return rel_text_start;
}

static inline void *Rel_getTextSectionEnd(void) {
    return rel_text_end;
}

static inline void *Rel_getRodataSectionEnd(void) {
    return rel_rodata_end;
}
