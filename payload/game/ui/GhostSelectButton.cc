#include "GhostSelectButton.hh"

#include "game/ui/TimeAttackGhostListPage.hh"
extern "C" {
#include "game/util/Registry.h"
}
#include "game/system/SaveManager.hh"

#include <stdio.h>

namespace UI {
enum {
    GROUP_ID_LOOP = 0x0,
    GROUP_ID_SELECT = 0x1,
    GROUP_ID_SELECT_IN = 0x2,
    GROUP_ID_OK = 0x3,
    GROUP_ID_CHOICE = 0x4,
};

// Loop
enum {
    ANIM_ID_LOOP = 0x0,
};

// Select
enum {
    ANIM_ID_FREE = 0x0,
    ANIM_ID_FREE_TO_SELECT = 0x1,
    ANIM_ID_SELECT = 0x2,
    ANIM_ID_SELECT_TO_FREE = 0x3,
};

// SelectIn
enum {
    ANIM_ID_SELECT_IN = 0x0,
    ANIM_ID_SELECT_STOP = 0x1,
};

// OK
enum {
    ANIM_ID_OK = 0x0,
    ANIM_ID_OK_STOP = 0x1,
};

// Choice
enum {
    ANIM_ID_CHOICE_OFF = 0x0,
    ANIM_ID_CHOICE_OFF_TO_ON = 0x1,
    ANIM_ID_CHOICE_ON = 0x2,
    ANIM_ID_CHOICE_ON_TO_OFF = 0x3,
};

TimeAttackGhostListPage *GhostSelectButton::getGhostListPage() {
    return reinterpret_cast<TimeAttackGhostListPage *>(UIControl::getPage());
}

void GhostSelectButton::onSelect(u32 localPlayerId, u32 /* r5 */) {
    OptionButton::onSelect(localPlayerId);

    TimeAttackGhostListPage *page = getGhostListPage();
    page->m_lastSelected = m_index;
}

void GhostSelectButton::onFront(u32 /* localPlayerId */, u32 /* r5 */) {
    m_animator.setAnimation(GROUP_ID_SELECT_IN, ANIM_ID_SELECT_IN, 0.0f);
    m_animator.setAnimation(GROUP_ID_OK, ANIM_ID_OK, 0.0f);

    TimeAttackGhostListPage *page = getGhostListPage();
    if (!m_chosen && page->m_chosenCount >= 11) {
        return;
    }

    m_chosen = !m_chosen;
    setChosen(m_chosen);
    if (m_chosen) {
        playSound(Sound::SoundId::SE_UI_TAB_OK, -1);
    }

    page->chooseGhost(m_index);
}

GhostSelectButton::GhostSelectButton() = default;

GhostSelectButton::~GhostSelectButton() = default;

void GhostSelectButton::load(u32 index) {
    char variant[0x10];
    snprintf(variant, sizeof(variant), "Option%u", index);
    OptionButton::load(index, "control", "GhostSelectOption", variant, false, false);
    setParentPane(variant);
    m_miiGroup.init(1, 0x1, NULL);
    setMiiPicture("mii_shadow", &m_miiGroup, 0, 0);
    setMiiPicture("mii", &m_miiGroup, 0, 0);
    setMiiPicture("active_mii", &m_miiGroup, 0, 0);
    setMiiPicture("mii_light_01", &m_miiGroup, 0, 0);
    setMiiPicture("mii_light_02", &m_miiGroup, 0, 0);
    m_inputManager.m_pane.selectHandler = &m_onSelect;
    m_inputManager.setHandler(MenuInputManager::InputId::Front, &m_onFront, false);
    m_animator.setAnimation(GROUP_ID_CHOICE, ANIM_ID_CHOICE_OFF, 0.0f);
}

void GhostSelectButton::refresh(u32 listIndex) {
    TimeAttackGhostListPage *page = getGhostListPage();
    u32 ghostIndex = page->m_ghostList->indices()[listIndex];
    auto *header = System::SaveManager::Instance()->rawGhostHeader(ghostIndex);

    m_miiGroup.insertFromRaw(0, &header->mii);
    MessageInfo nameInfo = {};
    nameInfo.miis[0] = m_miiGroup.get(0);

    setMessage("name_shadow", 9501, &nameInfo);
    setMessage("name", 9501, &nameInfo);
    setMessage("active_name", 9501, &nameInfo);
    setMessage("name_light_01", 9501, &nameInfo);
    setMessage("name_light_02", 9501, &nameInfo);

    char countryPane[0x4];
    snprintf(countryPane, sizeof(countryPane), "%03u", header->country);
    bool hasCountryPane = hasPictureSourcePane(countryPane);
    setPaneVisible("flag_shadow", hasCountryPane);
    setPaneVisible("flag", hasCountryPane);
    setPaneVisible("active_flag", hasCountryPane);
    setPaneVisible("flag_light_01", hasCountryPane);
    setPaneVisible("flag_light_02", hasCountryPane);
    if (hasCountryPane) {
        setPicture("flag_shadow", countryPane);
        setPicture("flag", countryPane);
        setPicture("active_flag", countryPane);
        setPicture("flag_light_01", countryPane);
        setPicture("flag_light_02", countryPane);
    }

    const char *characterPane = getCharacterPane(header->characterId);
    setPicture("chara_shadow", characterPane);
    setPicture("chara", characterPane);
    setPicture("active_chara", characterPane);
    setPicture("chara_light_01", characterPane);
    setPicture("chara_light_02", characterPane);

    char vehiclePane[0xa];
    snprintf(vehiclePane, sizeof(vehiclePane), "Vehicle%02u", header->vehicleId);
    setPicture("machine_shadow", vehiclePane);
    setPicture("machine", vehiclePane);
    setPicture("active_machine", vehiclePane);
    setPicture("machine_light_01", vehiclePane);
    setPicture("machine_light_02", vehiclePane);

    MessageInfo timeInfo = {};
    timeInfo.intVals[0] = header->raceTime.minutes;
    timeInfo.intVals[1] = header->raceTime.seconds;
    timeInfo.intVals[2] = header->raceTime.milliseconds;

    setMessage("time_shadow", 6052, &timeInfo);
    setMessage("time", 6052, &timeInfo);
    setMessage("active_time", 6052, &timeInfo);
    setMessage("time_light_01", 6052, &timeInfo);
    setMessage("time_light_02", 6052, &timeInfo);

    m_chosen = page->m_ghostIsChosen[ghostIndex];
    if (m_chosen) {
        setChosen(m_chosen);
    } else {
        m_animator.setAnimation(GROUP_ID_CHOICE, ANIM_ID_CHOICE_OFF, 0.0f);
    }
}

} // namespace UI
