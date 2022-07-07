#pragma once

#include "game/ui/Page.hh"

namespace UI {

class RacePage : public Page {
public:
    u32 watchedPlayerId() const;

    static TypeInfo *GetTypeInfo();

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

    u8 _044[0x064 - 0x044];
    u32 m_watchedPlayerId;
    u8 _068[0x1dc - 0x068];
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
