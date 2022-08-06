#pragma once

#include <Common.h>

typedef void (*GXDrawSyncCallback)(u16 token);

void GXSetDrawSync(u16 token);
GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback cb);

void GXDrawDone(void);
