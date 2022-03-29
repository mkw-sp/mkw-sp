#include "GhostSelectButton.h"

#include "game/system/SaveManager.h"
#include "game/ui/TimeAttackGhostListPage.h"
#include "game/util/Registry.h"

#include <stdio.h>

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

static void onSelect(InputHandler *this, u32 localPlayerId) {
    GhostSelectButton *button = CONTAINER_OF(this, GhostSelectButton, onSelect);
    TabOptionButton_onSelect(button, localPlayerId);

    TimeAttackGhostListPage *page = (TimeAttackGhostListPage *)button->group->page;
    page->lastSelected = button->index;
}

static const InputHandler_vt onSelect_vt = {
    .handle = onSelect,
};

static void onFront(InputHandler *this, u32 UNUSED(localPlayerId)) {
    GhostSelectButton *button = CONTAINER_OF(this, GhostSelectButton, onFront);
    UIAnimator_setAnimation(&button->animator, GROUP_ID_SELECT_IN, ANIM_ID_SELECT_IN, 0.0f);
    UIAnimator_setAnimation(&button->animator, GROUP_ID_OK, ANIM_ID_OK, 0.0f);

    TimeAttackGhostListPage *page = (TimeAttackGhostListPage *)button->group->page;
    if (!button->chosen && page->chosenCount >= 11) {
        return;
    }

    button->chosen = !button->chosen;
    TabOptionButton_setChosen(button, button->chosen);
    if (button->chosen) {
        UIControl_playSfx(button, 0xd, -1);
    }

    TimeAttackGhostListPage_chooseGhost(page, button->index);
}

static const InputHandler_vt onFront_vt = {
    .handle = onFront,
};

GhostSelectButton *GhostSelectButton_ct(GhostSelectButton *this) {
    TabOptionButton_ct(this);

    MiiGroup_ct(&this->miiGroup);
    this->onSelect.vt = &onSelect_vt;
    this->onFront.vt = &onFront_vt;

    return this;
}

void GhostSelectButton_dt(GhostSelectButton *this, s32 type) {
    MiiGroup_dt(&this->miiGroup, -1);

    TabOptionButton_dt(this, 0);
    if (type > 0) {
        delete(this);
    }
}

void GhostSelectButton_load(GhostSelectButton *this, u32 index) {
    char variant[0x10];
    snprintf(variant, sizeof(variant), "Option%lu", index);
    TabOptionButton_load(this, index, "control", "GhostSelectOption", variant, 0x1, false, false);
    LayoutUIControl_setParentPane(this, variant);
    MiiGroup_init(&this->miiGroup, 1, 0x1, NULL);
    LayoutUIControl_setMiiPicture(this, "mii_shadow", &this->miiGroup, 0, 0);
    LayoutUIControl_setMiiPicture(this, "mii", &this->miiGroup, 0, 0);
    LayoutUIControl_setMiiPicture(this, "active_mii", &this->miiGroup, 0, 0);
    LayoutUIControl_setMiiPicture(this, "mii_light_01", &this->miiGroup, 0, 0);
    LayoutUIControl_setMiiPicture(this, "mii_light_02", &this->miiGroup, 0, 0);
    this->inputManager.pane.selectHandler = &this->onSelect;
    ControlInputManager_setHandler(&this->inputManager, INPUT_ID_FRONT, &this->onFront, false);
    UIAnimator_setAnimation(&this->animator, GROUP_ID_CHOICE, ANIM_ID_CHOICE_OFF, 0.0f);
}

void GhostSelectButton_refresh(GhostSelectButton *this, u32 listIndex) {
    const TimeAttackGhostListPage *page = (TimeAttackGhostListPage *)this->group->page;
    u32 ghostIndex = page->ghostList->indices[listIndex];
    const RawGhostHeader *header = &s_saveManager->rawGhostHeaders[ghostIndex];

    MiiGroup_insertFromRaw(&this->miiGroup, 0, &header->mii);
    MessageInfo nameInfo = {
        .miis[0] = MiiGroup_get(&this->miiGroup, 0),
    };
    LayoutUIControl_setMessage(this, "name_shadow", 9501, &nameInfo);
    LayoutUIControl_setMessage(this, "name", 9501, &nameInfo);
    LayoutUIControl_setMessage(this, "active_name", 9501, &nameInfo);
    LayoutUIControl_setMessage(this, "name_light_01", 9501, &nameInfo);
    LayoutUIControl_setMessage(this, "name_light_02", 9501, &nameInfo);

    char countryPane[0x4];
    snprintf(countryPane, sizeof(countryPane), "%03lu", header->country);
    bool hasCountryPane = LayoutUIControl_hasPictureSourcePane(this, countryPane);
    LayoutUIControl_setPaneVisible(this, "flag_shadow", hasCountryPane);
    LayoutUIControl_setPaneVisible(this, "flag", hasCountryPane);
    LayoutUIControl_setPaneVisible(this, "active_flag", hasCountryPane);
    LayoutUIControl_setPaneVisible(this, "flag_light_01", hasCountryPane);
    LayoutUIControl_setPaneVisible(this, "flag_light_02", hasCountryPane);
    if (hasCountryPane) {
        LayoutUIControl_setPicture(this, "flag_shadow", countryPane);
        LayoutUIControl_setPicture(this, "flag", countryPane);
        LayoutUIControl_setPicture(this, "active_flag", countryPane);
        LayoutUIControl_setPicture(this, "flag_light_01", countryPane);
        LayoutUIControl_setPicture(this, "flag_light_02", countryPane);
    }

    const char *characterPane = getCharacterPane(header->characterId);
    LayoutUIControl_setPicture(this, "chara_shadow", characterPane);
    LayoutUIControl_setPicture(this, "chara", characterPane);
    LayoutUIControl_setPicture(this, "active_chara", characterPane);
    LayoutUIControl_setPicture(this, "chara_light_01", characterPane);
    LayoutUIControl_setPicture(this, "chara_light_02", characterPane);

    char vehiclePane[0xa];
    snprintf(vehiclePane, sizeof(vehiclePane), "Vehicle%02u", header->vehicleId);
    LayoutUIControl_setPicture(this, "machine_shadow", vehiclePane);
    LayoutUIControl_setPicture(this, "machine", vehiclePane);
    LayoutUIControl_setPicture(this, "active_machine", vehiclePane);
    LayoutUIControl_setPicture(this, "machine_light_01", vehiclePane);
    LayoutUIControl_setPicture(this, "machine_light_02", vehiclePane);

    MessageInfo timeInfo = {
        .intVals[0] = header->raceTime.minutes,
        .intVals[1] = header->raceTime.seconds,
        .intVals[2] = header->raceTime.milliseconds,
    };
    LayoutUIControl_setMessage(this, "time_shadow", 6052, &timeInfo);
    LayoutUIControl_setMessage(this, "time", 6052, &timeInfo);
    LayoutUIControl_setMessage(this, "active_time", 6052, &timeInfo);
    LayoutUIControl_setMessage(this, "time_light_01", 6052, &timeInfo);
    LayoutUIControl_setMessage(this, "time_light_02", 6052, &timeInfo);

    this->chosen = page->ghostIsChosen[listIndex];
    TabOptionButton_setChosen(this, this->chosen);
}
