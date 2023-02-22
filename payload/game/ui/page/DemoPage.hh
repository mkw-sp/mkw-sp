#pragma once

#include "game/ui/Page.hh"
#include "game/ui/GlobalContext.hh"

namespace UI {

class DemoPage: public Page {
private:
    REPLACE void onInit() override;

    LayoutUIControl m_courseDisplay;
    LayoutUIControl m_cupDisplay;
    MenuInputManager m_inputManager;
};

static_assert(sizeof(DemoPage) == 0x33c);

}