#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0xd8 - 0x00];
} TicketView;
static_assert(sizeof(TicketView) == 0xd8);

typedef struct {
    u8 _0000[0x0058 - 0x0000];
    u16 titleVersion;
    u8 _005a[0x205c - 0x005a];
} TmdView;
static_assert(sizeof(TmdView) == 0x205c);

s32 ESP_InitLib();
s32 ESP_GetTicketViews(u64 titleID, TicketView *views, u32 *count);
s32 ESP_GetTmdView(u64 titleID, TmdView *view, u32 *size);
s32 ESP_ListTitleContentsOnCard(u64 titleID, u32 *contentIds, u32 *count);
s32 ESP_Sign(const void *data, u32 size, u8 signature[0x3c], u8 certificate[0x180]);
