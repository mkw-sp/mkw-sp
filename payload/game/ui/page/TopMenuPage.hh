#pragma once

#include "game/ui/page/MenuPage.hh"

namespace UI {

class TopMenuPage : public MenuPage {
public:
    TopMenuPage();
    virtual ~TopMenuPage();

    void REPLACED(onInit)();
    REPLACE void onInit() override;

    REPLACE void onButtonSelect(PushButton *button);

    void initMiiGroup();
    void refreshFileAdminButton();

private:
    u8 _430[0xcb4 - 0x430];
    PushButton *m_fileAdminButton;

    static const char *s_buttonNames[4];
};
static_assert(sizeof(TopMenuPage) == 0xcb8);

} // namespace UI
