#pragma once

#include "Storage.h"

bool NetStorage_init(Storage *storage);

bool NetStorage_connect(u8 ipa, u8 ipb, u8 ipc, u8 ipd, u32 port);
void NetStorage_disconnect(void);
