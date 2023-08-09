#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"
#include "game/ui/UpDownControl.hh"
#include "game/util/Registry.hh"

namespace UI {

class RankingPage : public Page {
public:
    enum class Area {
        Friend,
        Regional,
        Worldwide,
    };

    RankingPage *construct();
    RankingPage();

    void REPLACED(onInit)();
    REPLACE void onInit() override;

    void REPLACED(onCourseChange)(UpDownControl *upDownControl, u32 localPlayerId, s32 index);
    REPLACE void onCourseChange(UpDownControl *upDownControl, u32 localPlayerId, s32 index);

    void REPLACED(handleGhostDownload)(f32 delay, u32 ghostType, u32 licenseId, s32 miiIndex);
    REPLACE void handleGhostDownload(f32 delay, u32 ghostType, u32 licenseId, s32 miiIndex);
    REPLACE void handleTopTenDownload(f32 delay);
    REPLACE void onBack(PushButton *pushButton, u32 localPlayerId);
    REPLACE void onBack();

    UpDownControl &courseControl();
    Area area() const;
    Registry::Course course() const;
    u32 ghostType() const;

private:
    u8 _0044[0x031C - 0x0044];
    UpDownControl m_courseControl;
    u8 _08E4[0x0D58 - 0x08E4];
    UpDownControl m_areaControl;
    u8 _1320[0x1B70 - 0x1320];
    PageId m_replacement;
    Area m_area;
    Registry::Course m_course;
    u8 _1B7C[0x1C28 - 0x1B7C];

    // Added
    u32 m_ghostType;
};
static_assert(sizeof(RankingPage) == 0x1C28 + sizeof(u32));

} // namespace UI
