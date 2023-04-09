#include "MenuPage.hh"

namespace UI {

class TopMenuPage : public MenuPage {
public:
    TopMenuPage();
    virtual ~TopMenuPage();

    REPLACE void onButtonSelect(PushButton *button);

    void initMiiGroup();
    void refreshFileAdminButton();

private:
    u8 _430[0xcb4 - 0x430];
    PushButton *m_fileAdminButton;
};

static_assert(sizeof(TopMenuPage) == 0xcb8);

} // namespace UI
