#pragma once

#include "game/ui/Button.hh"
#include "game/ui/ConfirmPage.hh"
#include "game/ui/UpDownControl.hh"

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

    REPLACE void handleTopTenDownload(f32 delay);
    REPLACE void onBack(PushButton *pushButton, u32 localPlayerId);
    REPLACE void onBack();

    UpDownControl &courseControl();

private:
    void pop(ConfirmPage *confirmPage, f32 delay);

    template <typename T>
    using H = typename T::template Handler<RankingPage>;

    u8 _0044[0x031C - 0x0044];
    UpDownControl m_courseControl;
    u8 _08E4[0x0D58 - 0x08E4];
    UpDownControl m_areaControl;
    u8 _1320[0x1B70 - 0x1320];
    PageId m_replacement;
    u8 _1B74[0x1C28 - 0x1B74];

    // Added
    H<ConfirmPage> m_pop{this, &RankingPage::pop};
};

} // namespace UI
