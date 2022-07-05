#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class CtrlRaceBase : public LayoutUIControl {
public:
    CtrlRaceBase();
    ~CtrlRaceBase() override;
    void initSelf() override;
    void vf_28() override;
    void vf_2c() override;

protected:
    virtual void process();
    virtual void vf_40();
    virtual void vf_44();
    virtual void vf_48();
    virtual void vf_4c();

    void setPaneColor(const char *pane, bool teamColors);

public:
    s8 REPLACED(getPlayerId)() const;
    REPLACE s8 getPlayerId() const;
    void calcLabelVisibility(const char *pane);

private:
    u8 _174[0x190 - 0x174];

protected:
    u8 m_localPlayerId;

private:
    u8 _191[0x198 - 0x191];
};
static_assert(sizeof(CtrlRaceBase) == 0x198);

} // namespace UI
