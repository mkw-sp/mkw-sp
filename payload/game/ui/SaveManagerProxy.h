#pragma once

#include <Common.h>

typedef struct {
    u8 _000[0x128 - 0x000];
} SaveManagerProxy;
static_assert(sizeof(SaveManagerProxy) == 0x128);

bool SaveManagerProxy_hasPendingRequests(const SaveManagerProxy *this);

void SaveManagerProxy_markLicensesDirty(SaveManagerProxy *this);
