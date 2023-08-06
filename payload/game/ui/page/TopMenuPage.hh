#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

class TopMenuPage : public MenuPage {
public:
    TopMenuPage();
    ~TopMenuPage() override;

    void REPLACED(onInit)();
    REPLACE void onInit() override;
    void REPLACED(vf_88)(int buttonIndex);
    REPLACE void vf_88(int buttonIndex) override;

    REPLACE void onButtonSelect(PushButton *button);
    void REPLACED(onButtonFront)(PushButton *button);
    REPLACE void onButtonFront(PushButton *button);

    void initMiiGroup();
    void initRaceConfig(s32);
    void refreshFileAdminButton();

private:
    u8 _430[0xcb4 - 0x430];
    PushButton *m_fileAdminButton;

    static const char *s_buttonNames[4];
};
static_assert(sizeof(TopMenuPage) == 0xcb8);

} // namespace UI
