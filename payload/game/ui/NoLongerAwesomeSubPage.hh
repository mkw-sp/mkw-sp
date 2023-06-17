#pragma once

#include "game/ui/Page.hh"

namespace UI {

class NoLongerAwesomeSubPage1 : public Page {
public:
    NoLongerAwesomeSubPage1();
    ~NoLongerAwesomeSubPage1() override;

    void onInit() override;
    PageId getReplacement() override;

private:
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<NoLongerAwesomeSubPage1>;

    PageInputManager m_inputManager;

    H<PageInputManager> m_onBack{this, &NoLongerAwesomeSubPage1::onBack};
};

class NoLongerAwesomeSubPage2 : public Page {
public:
    NoLongerAwesomeSubPage2();
    ~NoLongerAwesomeSubPage2() override;

    void onInit() override;
    PageId getReplacement() override;

private:
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<NoLongerAwesomeSubPage2>;

    PageInputManager m_inputManager;

    H<PageInputManager> m_onBack{this, &NoLongerAwesomeSubPage2::onBack};
};

class NoLongerAwesomeSubPage3 : public Page {
public:
    NoLongerAwesomeSubPage3();
    ~NoLongerAwesomeSubPage3() override;

    void onInit() override;
    PageId getReplacement() override;

private:
    void onBack(u32 localPlayerId);

    template <typename T>
    using H = typename T::template Handler<NoLongerAwesomeSubPage3>;

    PageInputManager m_inputManager;

    H<PageInputManager> m_onBack{this, &NoLongerAwesomeSubPage3::onBack};
};

} // namespace UI
