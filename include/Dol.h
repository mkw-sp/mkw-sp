#pragma once

#include <Common.h>

extern void dol_init_start;
extern void dol_sbss2_end;

static void *Dol_getStart(void) {
    return &dol_init_start;
}

static void *Dol_getEnd(void) {
    return &dol_sbss2_end;
}

static u32 Dol_getSize(void) {
    return &dol_sbss2_end - &dol_init_start;
}
