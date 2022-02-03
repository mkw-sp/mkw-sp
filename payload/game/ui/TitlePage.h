#pragma once

#include "Page.h"

typedef struct {
    Page;
    u8 _044[0x350 - 0x044];
} TitlePage;
static_assert(sizeof(TitlePage) == 0x350);
