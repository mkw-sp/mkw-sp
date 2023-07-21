#pragma once

#include "game/ui/Page.hh"

namespace UI {

class WU8LibraryPage : public Page {
public:
    void onInit() override;
    void onActivate() override;
    void afterCalc() override;

private:
    PageInputManager m_inputManager;
};

} // namespace UI
