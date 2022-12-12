#include "GhostSelectControl.hh"
#include "TimeAttackGhostListPage.hh"

namespace UI {

enum {
    GROUP_ID_SLIDE = 0x0,
};

// Slide
enum {
    ANIM_ID_HIDE = 0x0,
    ANIM_ID_SLIDE_IN_FROM_LEFT = 0x1,
    ANIM_ID_SLIDE_IN_FROM_RIGHT = 0x2,
    ANIM_ID_SHOW = 0x3,
    ANIM_ID_SLIDE_OUT_TO_LEFT = 0x4,
    ANIM_ID_SLIDE_OUT_TO_RIGHT = 0x5,
};

TimeAttackGhostListPage *GhostSelectControl::getGhostListPage() {
    return reinterpret_cast<TimeAttackGhostListPage *>(UIControl::getPage());
}

GhostSelectControl::GhostSelectControl() = default;
GhostSelectControl::~GhostSelectControl() = default;

u32 GhostSelectControl::getSlideAnimation() {
    return m_animator.getAnimation(GROUP_ID_SLIDE);
}

void GhostSelectControl::setSlideAnimation(u32 animationId) {
    m_animator.setAnimation(GROUP_ID_SLIDE, animationId, 0.0f);
}

bool GhostSelectControl::hasSlidIn() {
    switch (getSlideAnimation()) {
    case ANIM_ID_SLIDE_IN_FROM_LEFT:
    case ANIM_ID_SLIDE_IN_FROM_RIGHT:
        return !m_animator.getActive(GROUP_ID_SLIDE);
    default:
        return false;
    }
}

void GhostSelectControl::calcSelf() {
    if (hasSlidIn()) {
        for (auto &button : buttons) {
            if (button.getVisible()) {
                button.setPlayerFlags(0x1);
            }
        }
        TimeAttackGhostListPage *page = getGhostListPage();
        if (page->m_lastSelected == -1) {
            if (page->m_chosenCount == 0) {
                page->m_aloneButton.select(0);
            } else {
                switch (getSlideAnimation()) {
                case ANIM_ID_SLIDE_IN_FROM_LEFT:
                    page->m_raceButton.select(0);
                    break;
                case ANIM_ID_SLIDE_IN_FROM_RIGHT:
                    page->m_watchButton.select(0);
                    break;
                }
            }
        } else if (!buttons[page->m_lastSelected].getVisible()) {
            buttons[0].select(0);
        } else {
            buttons[page->m_lastSelected].select(0);
        }

        setSlideAnimation(ANIM_ID_SHOW);
    }

    setVisible(getSlideAnimation() != ANIM_ID_HIDE);
}

void GhostSelectControl::load() {
    const char *groups[] = {
        "Slide",
        "Hide",
        "SlideInFromLeft",
        "SlideInFromRight",
        "Show",
        "SlideOutToLeft",
        "SlideOutToRight",
        nullptr,
        nullptr,
    };
    LayoutUIControl::load("control", "GhostSelectBase", "Base", groups);
    m_animator.setAnimation(GROUP_ID_SLIDE, ANIM_ID_HIDE, 0.0f);

    initChildren(buttons.size());
    for (u32 i = 0; i < buttons.size(); ++i) {
        insertChild(i, &buttons[i]);
        buttons[i].load(i);
    }
}

bool GhostSelectControl::isHidden() {
    return getSlideAnimation() == ANIM_ID_HIDE;
}

bool GhostSelectControl::isShown() {
    return getSlideAnimation() == ANIM_ID_SHOW;
}

void GhostSelectControl::onHide() {
    for (auto &button : buttons) {
        button.setPlayerFlags(0x0);
    }
}

void GhostSelectControl::onShow() {
    TimeAttackGhostListPage *page = getGhostListPage();
    for (u32 i = 0; i < buttons.size(); ++i) {
        const u32 listIndex = page->m_sheetIndex * buttons.size() + i;
        if (listIndex < page->m_ghostList->count()) {
            buttons[i].setVisible(true);
            buttons[i].refresh(listIndex);
        } else {
            buttons[i].setVisible(false);
        }
    }
}

void GhostSelectControl::hide() {
    setSlideAnimation(ANIM_ID_HIDE);
    onHide();
}

void GhostSelectControl::slideInFromLeft() {
    setSlideAnimation(ANIM_ID_SLIDE_IN_FROM_LEFT);
    onShow();
}

void GhostSelectControl::slideInFromRight() {
    setSlideAnimation(ANIM_ID_SLIDE_IN_FROM_RIGHT);
    onShow();
}

void GhostSelectControl::show() {
    setSlideAnimation(ANIM_ID_SHOW);
    onShow();
    for (u32 i = 0; i < buttons.size(); ++i) {
        u32 flags = buttons[i].getVisible() ? 0x1 : 0x0;
        buttons[i].setPlayerFlags(flags);
    }
}

void GhostSelectControl::slideOutToLeft() {
    setSlideAnimation(ANIM_ID_SLIDE_OUT_TO_LEFT);
    onHide();
}

void GhostSelectControl::slideOutToRight() {
    setSlideAnimation(ANIM_ID_SLIDE_OUT_TO_RIGHT);
    onHide();
}

}  // namespace UI
