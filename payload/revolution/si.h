#pragma once

#include <Common.h>
#include <sp/ScopeLock.h>

typedef void (*SICallback)(s32, u32, void *);
typedef void (*SITypeAndStatusCallback)(s32, u32);

bool SIBusy(void);
bool SIIsChanBusy(s32);
bool SIEnablePollingInterrupt(bool);
bool SIUnregisterPollingHandler(void *);
u32 SIGetStatus(s32);
void SISetCommand(s32, u32);
void SITransferCommands(void);
u32 SIEnablePolling(u32);
u32 SIDisablePolling(u32);
bool SITransfer(s32, void *, u32, void *, u32, SICallback, s64);
void SIRefreshSamplingRate(void);
bool SIGetResponse(s32, void *);
u32 SIGetType(s32);
u32 SIGetTypeAsync(s32, SITypeAndStatusCallback);

void SISetSamplingRate(u32 rate);

extern void *RDSTHandler[4];

static inline void **FindRDSTHandler(void *handler) {
    for (size_t i = 0; i < ARRAY_SIZE(RDSTHandler); ++i) {
        if (RDSTHandler[i] == handler) {
            return &RDSTHandler[i];
        }
    }

    return NULL;
}

static inline bool SIRegisterPollingHandler(void *handler) {
    SP_SCOPED_NO_INTERRUPTS();

    if (FindRDSTHandler(handler) != NULL) {
        return true;
    }

    void **slot = FindRDSTHandler(NULL);
    if (slot == NULL) {
        return false;
    }

    *slot = handler;
    SIEnablePollingInterrupt(true);
    return true;
}

#define SI_CHAN_BIT(n) (0x80000000 >> n)

#define SI_ERROR_UNDER_RUN 0x0001
#define SI_ERROR_OVER_RUN 0x0002
#define SI_ERROR_COLLISION 0x0004
#define SI_ERROR_NO_RESPONSE 0x0008
#define SI_ERROR_WRST 0x0010
#define SI_ERROR_RDST 0x0020
#define SI_ERROR_UNKNOWN 0x0040
#define SI_ERROR_BUSY 0x0080
