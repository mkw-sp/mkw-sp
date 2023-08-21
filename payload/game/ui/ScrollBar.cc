#include "ScrollBar.hh"

#include "game/ui/Page.hh"

#include <cmath>

namespace UI {

namespace GroupId {

enum {
    Loop = 0,
    Select = 1,
};

} // namespace GroupId

namespace AnimId::Loop {

enum {
    Loop = 0,
};

} // namespace AnimId::Loop

namespace AnimId::Select {

enum {
    Free = 0,
    FreeToSelect = 1,
    Select = 2,
    SelectToFree = 3,
};

} // namespace AnimId::Select

ScrollBar::ScrollBar() = default;

ScrollBar::~ScrollBar() = default;

void ScrollBar::calcSelf() {
    bool loopActive = m_animator.getAnimation(GroupId::Select) != AnimId::Select::Free;
    m_animator.setActive(GroupId::Loop, loopActive);

    auto *fuchiPatternMaterial = m_fuchiPatternPane->getMaterial();
    assert(fuchiPatternMaterial);
    m_inputManager.getFuchiPatternColors(&fuchiPatternMaterial->tevColors[0],
            &fuchiPatternMaterial->tevColors[1]);

    GXColorS10 colorBaseColor;
    m_inputManager.getColorBaseColor(&colorBaseColor);
    auto *colorBaseMaterial = m_colorBasePane->getMaterial();
    assert(colorBaseMaterial);
    colorBaseMaterial->tevColors[0] = colorBaseColor;
    colorBaseMaterial->tevColors[1] = colorBaseColor;

    if (m_sequence) {
        auto *parentInputManager = getPage()->inputManager()->downcast<MultiControlInputManager>();
        assert(parentInputManager);
        u32 flags = parentInputManager->sequenceFlags(m_sequence->localPlayerId);
        flags &= 1 << 3 | 1 << 2;
        if (m_sequence->flags == 0 || flags == m_sequence->flags) {
            if (m_sequence->frames % 5 == 0) {
                if (flags & 1 << 2) {
                    if (m_chosen != m_count - 1) {
                        m_chosen++;
                        if (m_changeHandler) {
                            m_changeHandler->handle(this, m_sequence->localPlayerId, m_chosen);
                        }

                        playSound(Sound::SoundId::SE_UI_UD_RIGHT, m_sequence->localPlayerId);
                    }
                } else if (flags & 1 << 3) {
                    if (m_chosen != 0) {
                        m_chosen--;
                        if (m_changeHandler) {
                            m_changeHandler->handle(this, m_sequence->localPlayerId, m_chosen);
                        }

                        playSound(Sound::SoundId::SE_UI_UD_LEFT, m_sequence->localPlayerId);
                    }
                }
            }
            m_sequence->frames++;
        } else {
            m_sequence->frames = 0;
        }
        m_sequence->flags = flags;
    }

    m_thumbFuchiPane->m_trans.x = static_cast<s32>(
            (m_colorBasePane->m_width - 4.0f) * ((m_chosen + 0.5f) / m_count - 0.5f));
    m_thumbPane->m_trans.x = m_thumbFuchiPane->m_trans.x;
}

void ScrollBar::load(u32 count, u32 chosen, const char *dir, const char *file, const char *variant,
        u32 playerFlags, bool isMultiPlayer, bool pointerOnly) {
    // clang-format off
    const char *groups[] = {
        "Loop", "Loop", nullptr,
        "Select", "Free", "FreeToSelect", "Select", "SelectToFree", nullptr,
        nullptr,
    };
    // clang-format on

    LayoutUIControl::load(dir, file, variant, groups);

    m_inputManager.m_pane.pane = m_mainLayout.findPaneByName("touch");
    assert(m_inputManager.m_pane.pane);
    m_inputManager.m_pane.selectHandler = &m_onSelect;
    m_inputManager.m_pane.deselectHandler = &m_onDeselect;
    m_inputManager.m_playerFlags = playerFlags;
    m_inputManager.setHandler(MenuInputManager::InputId::Front, &m_onFront, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Right, &m_onRight, false);
    m_inputManager.setHandler(MenuInputManager::InputId::Left, &m_onLeft, false);
    m_inputManager.init(0, isMultiPlayer, pointerOnly);
    auto *parentInputManager = getPage()->inputManager()->downcast<MultiControlInputManager>();
    assert(parentInputManager);
    parentInputManager->append(&m_inputManager);

    m_fuchiPatternPane = m_mainLayout.findPaneByName("fuchi_pattern");
    assert(m_fuchiPatternPane);
    m_colorBasePane = m_mainLayout.findPaneByName("color_base");
    assert(m_colorBasePane);
    m_thumbFuchiPane = m_mainLayout.findPaneByName("thumb_fuchi");
    assert(m_thumbFuchiPane);
    m_thumbPane = m_mainLayout.findPaneByName("thumb");
    assert(m_thumbPane);

    m_animator.setAnimationInactive(GroupId::Loop, AnimId::Loop::Loop, 0.0f);
    m_animator.setAnimationInactive(GroupId::Select, AnimId::Select::Free, 0.0f);

    reconfigure(count, chosen, 0x1);
}

void ScrollBar::setChangeHandler(IHandler *handler) {
    m_changeHandler = handler;
}

void ScrollBar::setPlayerFlags(u32 flags) {
    m_inputManager.m_playerFlags = flags;
}

void ScrollBar::reconfigure(u32 count, u32 chosen, u32 playerFlags) {
    m_count = count;
    m_chosen = chosen;

    m_thumbFuchiPane->m_width = std::max((m_colorBasePane->m_width - 4.0f) / m_count, 5.0f);
    m_thumbPane->m_width = m_thumbFuchiPane->m_width - 4.5f;

    setVisible(m_count >= 4);
    setPlayerFlags(playerFlags);
}

void ScrollBar::onSelect(u32 localPlayerId, u32 /* r5 */) {
    auto *group = m_animator.getGroup(GroupId::Select);
    if (group->getAnimation() == AnimId::Select::Free) {
        group->setAnimation(AnimId::Select::FreeToSelect, 0.0f);
    } else if (group->getAnimation() == AnimId::Select::SelectToFree) {
        f32 frame = (1.0f - group->getFrame() / group->getDuration(AnimId::Select::SelectToFree)) *
                group->getDuration(AnimId::Select::FreeToSelect);
        group->setAnimation(AnimId::Select::FreeToSelect, frame);
    }

    auto *parentInputManager = getPage()->inputManager()->downcast<MultiControlInputManager>();
    assert(parentInputManager);
    u32 flags = parentInputManager->sequenceFlags(localPlayerId);
    flags &= 1 << 3 | 1 << 2;
    m_sequence.emplace(localPlayerId, flags, 0);

    playSound(Sound::SoundId::SE_UI_UD_IN, localPlayerId);
}

void ScrollBar::onDeselect(u32 /* localPlayerId */, u32 /* r5 */) {
    auto *group = m_animator.getGroup(GroupId::Select);
    if (group->getAnimation() == AnimId::Select::Select) {
        group->setAnimation(AnimId::Select::SelectToFree, 0.0f);
    } else if (group->getAnimation() == AnimId::Select::FreeToSelect) {
        f32 frame = (1.0f - group->getFrame() / group->getDuration(AnimId::Select::FreeToSelect)) *
                group->getDuration(AnimId::Select::SelectToFree);
        group->setAnimation(AnimId::Select::SelectToFree, frame);
    }

    m_sequence.reset();
}

void ScrollBar::onFront(u32 localPlayerId, u32 /* r5 */) {
    auto *parentInputManager = getPage()->inputManager()->downcast<MultiControlInputManager>();
    assert(parentInputManager);
    if (!parentInputManager->isPointer(localPlayerId)) {
        return;
    }
    auto pointerPos = parentInputManager->pointerPos(localPlayerId);
    f32 width = m_colorBasePane->m_width;
    f32 height = m_colorBasePane->m_height;
    f32 localPanePos0[3] = {-0.5f * (width - 4.0f), -0.5f * (height + 4.0f), 0.0f};
    f32 globalPanePos0[3];
    PSMTXMultVec(m_colorBasePane->m_globalMtx, localPanePos0, globalPanePos0);
    f32 localPanePos1[3] = {0.5f * (width - 4.0f), 0.5f * (height + 4.0f), 0.0f};
    f32 globalPanePos1[3];
    PSMTXMultVec(m_colorBasePane->m_globalMtx, localPanePos1, globalPanePos1);
    if (pointerPos.y >= globalPanePos0[1] && pointerPos.y < globalPanePos1[1]) {
        f32 t = (pointerPos.x - globalPanePos0[0]) / (globalPanePos1[0] - globalPanePos0[0]);
        s32 chosen = t * m_count;
        if (chosen >= 0 && static_cast<u32>(chosen) < m_count &&
                static_cast<u32>(chosen) != m_chosen) {
            m_chosen = chosen;
            if (m_changeHandler) {
                m_changeHandler->handle(this, m_sequence->localPlayerId, m_chosen);
            }
        }
    }

    playSound(Sound::SoundId::SE_UI_BTN_OK, localPlayerId);
}

void ScrollBar::onRight(u32 /* localPlayerId */, u32 /* r5 */) {}

void ScrollBar::onLeft(u32 /* localPlayerId */, u32 /* r5 */) {}

} // namespace UI
