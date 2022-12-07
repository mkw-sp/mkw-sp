#pragma once

#include <Common.h>

extern u8 payload_text_start[];
extern u8 payload_text_end[];
extern u8 payload_replacements_start[];
extern u8 payload_replacements_end[];
extern u8 payload_patches_start[];
extern u8 payload_patches_end[];
extern u8 payload_rodata_start[];
extern u8 payload_rodata_end[];
extern u8 payload_data_start[];
extern u8 payload_data_end[];

static inline void *Payload_getEnd(void) {
    return payload_data_end;
}

static inline size_t Payload_getSize(void) {
    return payload_data_end - payload_text_start;
}

static inline void *Payload_getTextSectionStart(void) {
    return payload_text_start;
}

static inline void *Payload_getTextSectionEnd(void) {
    return payload_text_end;
}

static inline void *Payload_getReplacementsSectionStart(void) {
    return payload_replacements_start;
}

static inline void *Payload_getReplacementsSectionEnd(void) {
    return payload_replacements_end;
}

static inline void *Payload_getPatchesSectionStart(void) {
    return payload_patches_start;
}

static inline void *Payload_getPatchesSectionEnd(void) {
    return payload_patches_end;
}

static inline void *Payload_getRodataSectionStart(void) {
    return payload_rodata_start;
}

static inline void *Payload_getRodataSectionEnd(void) {
    return payload_rodata_end;
}

static inline void *Payload_getDataSectionStart(void) {
    return payload_data_start;
}

static inline void *Payload_getDataSectionEnd(void) {
    return payload_data_end;
}

void Payload_Init(void);
