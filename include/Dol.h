#pragma once

#include <Common.h>

extern void dol_start;
extern void dol_end;

static void *Dol_getStart(void) {
    return &dol_start;
}

static void *Dol_getEnd(void) {
    return &dol_end;
}

static u32 Dol_getSize(void) {
    return &dol_end - &dol_start;
}
