#pragma once

typedef struct OSContext {
    u32 gprs[32];

    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;

    f64 fprs[32];

    u32 _190;
    u32 fpscr;

    u32 srr0;
    u32 srr1;

    u16 _1A0;
    u16 _1A2;

    u32 gqrs[8];
    u32 _1C4;
    f64 psfs[32];
} OSContext;
static_assert(sizeof(OSContext) == 0x2C8);
