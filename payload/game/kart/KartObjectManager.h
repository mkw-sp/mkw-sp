#include "KartObject.h"

typedef struct {
    u8 _00[0x20 - 0x00];
    KartObject **objects;
    u8 _24[0x38 - 0x24];
} KartObjectManager;

extern KartObjectManager *s_kartObjectManager;

void KartObjectManager_createInstance(void);

extern bool speedModIsEnabled;
extern f32 speedModFactor;
extern f32 speedModReverseFactor;
