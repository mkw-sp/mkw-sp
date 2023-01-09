#pragma once

#include <Common.h>

//! Bidirectional list node
typedef struct {
    void *pred;
    void *succ;
} ut_Node;

typedef struct {
    u8 _0[0xc - 0x0];
} ut_List;

void ut_List_Append(ut_List *list, void *elem);
void ut_List_Remove(ut_List *list, void *elem);
