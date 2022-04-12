#pragma once

#include <Common.h>

extern u8 dol_start[];
extern u8 dol_end[];
extern u8 dol_init_start[];
extern u8 dol_init_end[];
extern u8 dol_text_start[];
extern u8 dol_text_end[];
extern u8 dol_rodata_end[];
extern u8 dol_sdata2_start[];
extern u8 dol_sbss2_end[];

static inline void *Dol_getStart(void) {
    return dol_start;
}

static inline void *Dol_getEnd(void) {
    return dol_end;
}

static inline u32 Dol_getSize(void) {
    return dol_end - dol_start;
}

static inline void *Dol_getInitSectionStart(void) {
    return dol_init_start;
}

static inline void *Dol_getInitSectionEnd(void) {
    return dol_init_end;
}

static inline void *Dol_getTextSectionStart(void) {
    return dol_text_start;
}

static inline void *Dol_getTextSectionEnd(void) {
    return dol_text_end;
}

static inline void *Dol_getRodataSectionEnd(void) {
    return dol_rodata_end;
}

static inline void *Dol_getSdata2SectionStart(void) {
    return dol_sdata2_start;
}

static inline void *Dol_getSbss2SectionEnd(void) {
    return dol_sbss2_end;
}
