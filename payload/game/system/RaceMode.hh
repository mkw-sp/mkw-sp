#pragma once

#include "game/system/RaceManager.hh"

namespace System {

class RaceMode {
public:
    RaceMode();
    virtual ~RaceMode();
    virtual bool canEndRace();
    virtual void endRace();
    virtual void calc();
    virtual void calcPosition();
    virtual void getJugemPoint();
    virtual void init();
    virtual void vf_20();
    virtual void tryEndRace();
    virtual void vf_28();

protected:
    RaceManager *m_raceManager;
};
static_assert(sizeof(RaceMode) == 0x8);

class TimeAttackRaceMode : public RaceMode {
public:
    TimeAttackRaceMode();
    virtual ~TimeAttackRaceMode();

    REPLACE bool canEndRace() override;
};
static_assert(sizeof(TimeAttackRaceMode) == sizeof(RaceMode));

class BattleRaceMode : public RaceMode {
public:
    // ...
    virtual void onRemoveHit(u32 sender, u32 receiver);
    virtual void onMoveHit(u32 sender, u32 receiver);
    virtual void onRemoveHit(u32 receiver);
    virtual void onSpineRemoveHit(u32 receiver);
    virtual void vf_3c();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();

private:
    u8 _08[0x38 - 0x08];
};
static_assert(sizeof(BattleRaceMode) == 0x38);

class BalloonBattleRaceMode : public BattleRaceMode {
public:
    ~BalloonBattleRaceMode() override;
    void REPLACED(onRemoveHit)(u32 sender, u32 receiver);
    REPLACE void onRemoveHit(u32 sender, u32 receiver) override;
    void REPLACED(onMoveHit)(u32 sender, u32 receiver);
    REPLACE void onMoveHit(u32 sender, u32 receiver) override;
    void onRemoveHit(u32 receiver) override;
    // ...
};
static_assert(sizeof(BalloonBattleRaceMode) == sizeof(BattleRaceMode));

class CoinBattleRaceMode : public BattleRaceMode {
public:
    ~CoinBattleRaceMode() override;
    void REPLACED(onRemoveHit)(u32 sender, u32 receiver);
    REPLACE void onRemoveHit(u32 sender, u32 receiver) override;
    void REPLACED(onMoveHit)(u32 sender, u32 receiver);
    REPLACE void onMoveHit(u32 sender, u32 receiver) override;
    void onRemoveHit(u32 receiver) override;
    // ...
};
static_assert(sizeof(CoinBattleRaceMode) == sizeof(BattleRaceMode));

} // namespace System
