#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlRaceWifiFinishMessage.hh"
#include "game/ui/ctrl/CtrlRaceWifiStartMessage.hh"

namespace UI {

class RacePage : public Page {
public:
    ~RacePage() override;
    REPLACE PageId getReplacement() override;
    void REPLACED(onInit)();
    REPLACE void onInit() override;
    void REPLACED(afterCalc)();
    REPLACE void afterCalc() override;
    // ...

    u32 watchedPlayerId() const;
    u32 lastWatchedPlayerId() const;

    static TypeInfo *GetTypeInfo();
    static RacePage *Instance();

protected:
    virtual void vf_64();
    virtual u32 getControls() const;
    virtual u32 getNameBalloonCount() const;
    virtual void vf_70();
    virtual void vf_74();
    virtual void vf_78();
    virtual void vf_7c();

private:
    u8 REPLACED(getControlCount)(u32 controls) const;
    REPLACE u8 getControlCount(u32 controls) const;
    void REPLACED(initControls)(u32 controls);
    REPLACE void initControls(u32 controls);

    PageId m_replacement;
    u8 _048[0x05c - 0x048];
    LayoutUIControl *m_ghostMessage;
    u8 _060[0x064 - 0x060];
    u32 m_watchedPlayerId;
    u8 _068[0x069 - 0x068];
    u8 m_lastWatchedPlayerId; // Added (was padding)
    u8 _06a[0x070 - 0x06a];
    CtrlRaceWifiStartMessage **m_startMessages;
    CtrlRaceWifiFinishMessage **m_finishMessages;
    u8 _078[0x1dc - 0x078];

    static RacePage *s_instance;
};
static_assert(sizeof(RacePage) == 0x1dc);

class TARacePage : public RacePage {
public:
    TARacePage();
    ~TARacePage() override;
    // ...
    REPLACE u32 getControls() const override;
    // ...
};

class VSMultiRacePage : public RacePage {
public:
    VSMultiRacePage();
    ~VSMultiRacePage() override;
    // ...
    REPLACE u32 getControls() const override;
    // ...
};

class GhostReplayRacePage : public RacePage {
public:
    GhostReplayRacePage();
    ~GhostReplayRacePage() override;
    // ...
    REPLACE u32 getControls() const override;
    // ...
};

} // namespace UI
