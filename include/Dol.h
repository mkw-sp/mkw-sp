#pragma once

#include <Common.h>

extern void dol_start;
extern void dol_end;
extern void dol_init_start;
extern void dol_rodata_end;
extern void dol_sdata2_start;
extern void dol_sbss2_end;

static void *Dol_getStart(void) {
    return &dol_start;
}

static void *Dol_getEnd(void) {
    return &dol_end;
}

static u32 Dol_getSize(void) {
    return &dol_end - &dol_start;
}

static void *Dol_getInitSectionStart(void) {
    return &dol_init_start;
}

static void *Dol_getRodataSectionEnd(void) {
    return &dol_rodata_end;
}

static void *Dol_getSdata2SectionStart(void) {
    return &dol_sdata2_start;
}

static void *Dol_getSbss2SectionEnd(void) {
    return &dol_sbss2_end;
}
