#pragma once

#include <Common.h>

typedef struct {
    u8 _0[0xc - 0x0];
} ut_List;

void ut_List_Append(ut_List *list, void *elem);
