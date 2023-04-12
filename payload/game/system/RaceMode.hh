#pragma once

#include "RaceManager.hh"

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

} // namespace System
