#pragma once

#include "game/kart/KartObject.hh"
#include "game/ui/ctrl/CtrlRaceBase.hh"

namespace UI {

class CtrlRace2DMapObject : public CtrlRaceBase {
public:
    CtrlRace2DMapObject();
    ~CtrlRace2DMapObject() override;

    virtual void calcTransform(Vec3 *pos, void *r5, void *r6);
    virtual void vf_54();
    virtual void vf_58();
    virtual void vf_5c();

private:
    u8 _198[0x1b4 - 0x198];
};
static_assert(sizeof(CtrlRace2DMapObject) == 0x1b4);

class CtrlRace2DMapCharacter : public CtrlRace2DMapObject {
public:
    CtrlRace2DMapCharacter();
    ~CtrlRace2DMapCharacter() override;

    void REPLACED(calcTransform)(Vec3 *pos, void *r5, void *r6);
    REPLACE void calcTransform(Vec3 *pos, void *r5, void *r6) override;

    REPLACE virtual void load(u32 playerId);

private:
    u8 m_playerId;
    nw4r::lyt::Pane *m_charaPane;
    nw4r::lyt::Pane *m_charaShadow0Pane;
    nw4r::lyt::Pane *m_charaShadow1Pane;
    nw4r::lyt::Pane *m_lightPane;
    Kart::KartObject *m_object;
};
static_assert(sizeof(CtrlRace2DMapCharacter) == 0x1cc);

} // namespace UI
