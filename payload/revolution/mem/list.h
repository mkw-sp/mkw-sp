#pragma once

#include <Common.h>

typedef struct {
    void *prevObject;
    void *nextObject;
} MEMLink;

typedef struct {
    void *headObject;
    void *tailObject;
    u16 numObjects;
    u16 offset;
} MEMList;

void *MEMGetNextListObject(MEMList *list, void *object);
