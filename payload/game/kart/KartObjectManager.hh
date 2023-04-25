#pragma once

#include "game/kart/KartObject.hh"

extern bool speedModIsEnabled;
extern f32 speedModFactor;
extern f32 speedModReverseFactor;

namespace Kart {

class KartObjectManager {
public:
    KartObjectManager();
    virtual ~KartObjectManager();

    KartObject *object(u32 playerId);

    void beforeCalc();
    void calc();

    static REPLACE void CreateInstance();
    static KartObjectManager *Instance();

private:
    u8 _00[0x20 - 0x04];
    KartObject **m_objects;
    u8 m_count;
    u8 _25[0x38 - 0x25];

    static KartObjectManager *s_instance;
};

static_assert(sizeof(KartObjectManager) == 0x38);

} // namespace Kart
