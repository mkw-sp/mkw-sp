#pragma once

#include "Page.h"

enum {
    // +License
    SECTION_ID_GP = 0x1e,
    // +QuitConfirm
    // +License
    SECTION_ID_TIME_ATTACK = 0x1f,

    // +QuitConfirm
    // +License
    SECTION_ID_VS_1P = 0x20,
    // +License
    SECTION_ID_VS_2P = 0x21,
    // +License
    SECTION_ID_VS_3P = 0x22,
    // +License
    SECTION_ID_VS_4P = 0x23,

    // +QuitConfirm
    // +License
    SECTION_ID_TEAM_VS_1P = 0x24,
    // +License
    SECTION_ID_TEAM_VS_2P = 0x25,
    // +License
    SECTION_ID_TEAM_VS_3P = 0x26,
    // +License
    SECTION_ID_TEAM_VS_4P = 0x27,

    // +QuitConfirm
    // +License
    SECTION_ID_BATTLE_1P = 0x28,
    // +License
    SECTION_ID_BATTLE_2P = 0x29,
    // +License
    SECTION_ID_BATTLE_3P = 0x2a,
    // +License
    SECTION_ID_BATTLE_4P = 0x2b,

    // +License
    SECTION_ID_MR_REPLAY = 0x2c,
    // +License
    SECTION_ID_TOURNAMENT_REPLAY = 0x2d,
    // +License
    SECTION_ID_GP_REPLAY = 0x2e,
    // +QuitConfirm
    // +License
    SECTION_ID_TT_REPLAY = 0x2f,
    
    // +License
    SECTION_ID_GHOST_TA = 0x30,
    // +License
    SECTION_ID_GHOST_TA_ONLINE = 0x31,

    // +License
    SECTION_ID_GHOST_REPLAY_CHANNEL = 0x32,
    // +License
    SECTION_ID_GHOST_REPLAY_DL = 0x33,
    // +License
    SECTION_ID_GHOST_REPLAY = 0x34,


    SECTION_ID_MII_SELECT_CREATE = 0x45,
    SECTION_ID_MII_SELECT_CHANGE = 0x46,
    SECTION_ID_SINGLE_CHANGE_COURSE = 0x4a, // Modified
    SECTION_ID_SINGLE_CHANGE_GHOST_DATA = 0x4d, // Modified
    SECTION_ID_CHANNEL = 0x7a,
};

typedef struct {
    u32 id;
    u8 _004[0x008 - 0x004];
    Page *pages[PAGE_ID_MAX];
    u8 _354[0x408 - 0x354];
} Section;
static_assert(sizeof(Section) == 0x408);

Page *Section_createPage(u32 pageId);

void Section_addPages(Section *section, u32 sectionId);
void Section_addPage(Section *section, u32 pageId);

void Section_addActivePages(Section *section, u32 sectionId);
void Section_addActivePage(Section *section, u32 pageId);
