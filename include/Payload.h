#pragma once

extern void payload_text_start;
extern void payload_text_end;
extern void payload_patches_start;
extern void payload_patches_end;
extern void payload_rodata_start;
extern void payload_rodata_end;
extern void payload_data_start;
extern void payload_data_end;

static void *Payload_getTextSectionStart(void) {
    return &payload_text_start;
}

static void *Payload_getTextSectionEnd(void) {
    return &payload_text_end;
}

static void *Payload_getPatchesSectionStart(void) {
    return &payload_patches_start;
}

static void *Payload_getPatchesSectionEnd(void) {
    return &payload_patches_end;
}

static void *Payload_getRodataSectionStart(void) {
    return &payload_rodata_start;
}

static void *Payload_getRodataSectionEnd(void) {
    return &payload_rodata_end;
}

static void *Payload_getDataSectionStart(void) {
    return &payload_data_start;
}

static void *Payload_getDataSectionEnd(void) {
    return &payload_data_end;
}
