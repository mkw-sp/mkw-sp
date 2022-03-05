#pragma once

#include <Common.h>

typedef struct {
    u8 _00[0xd8 - 0x00];
} TicketView;
static_assert(sizeof(TicketView) == 0xd8);

bool Es_init(void);

void Es_launchTitle(u64 titleId, const TicketView *view);

bool Es_deinit(void);
