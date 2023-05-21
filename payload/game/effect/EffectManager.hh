#pragma once

#include "game/effect/KartEffect.hh"

#include <egg/util/eggEffectResource.hh>

namespace Effect {

class EffectManager {
public:
    void REPLACED(deinit)();
    REPLACE void deinit();
    void REPLACED(initRace)();
    REPLACE void initRace();
    void REPLACED(createKarts)();
    REPLACE void createKarts();
    void calc();

    KartEffect *getKartEffect(u32 i) const;

    static EffectManager *Instance();

private:
    u8 _000[0x068 - 0x000];
    KartEffect **m_karts;
    u8 _06c[0x9f4 - 0x06c];

    static EffectManager *s_instance;
    static EGG::EffectResource *s_raceSPResource;
};
static_assert(sizeof(EffectManager) == 0x9f4);

} // namespace Effect
