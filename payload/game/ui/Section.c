#include "Section.h"

#include "ChannelPage.h"
#include "GhostManagerPage.h"
#include "LicenseSelectPage.h"
#include "LicenseSettingsPage.h"
#include "ServicePackTopPage.h"
#include "TimeAttackGhostListPage.h"
#include "TimeAttackRulesPage.h"
#include <game/missionrun/MissionRun.h>

#include <revolution.h>

PATCH_S16(Section_createPage, 0x92e, sizeof_LicenseSelectPage);
PATCH_S16(Section_createPage, 0x9a6, sizeof(TimeAttackRulesPage));
PATCH_S16(Section_createPage, 0xa4e, sizeof_TimeAttackGhostListPage);
PATCH_S16(Section_createPage, 0xee6, sizeof_ServicePackTopPage);
PATCH_S16(Section_createPage, 0xf5e, sizeof(GhostManagerPage));
PATCH_S16(Section_createPage, 0x1276, sizeof_ChannelPage);
PATCH_S16(Section_createPage, 0x12d6, sizeof(LicenseSettingsPage));

extern u8 ChannelTopPage_ct;
extern u8 ServicePackTopPage_ct;
PATCH_B(ChannelTopPage_ct, ServicePackTopPage_ct);

extern u8 LicenseSelectPage_ct;
extern u8 my_LicenseSelectPage_ct;
PATCH_B(LicenseSelectPage_ct, my_LicenseSelectPage_ct);

extern u8 TimeAttackGhostListPage_ct;
extern u8 my_TimeAttackGhostListPage_ct;
PATCH_B(TimeAttackGhostListPage_ct, my_TimeAttackGhostListPage_ct);

extern u8 ChannelExplanationPage_ct;
extern u8 ChannelPage_ct;
PATCH_B(ChannelExplanationPage_ct, ChannelPage_ct);

// The game has 5 pages for the records, we only need 1 for the settings. Disable the 4
// others.
#define ONLY_ONE_RECORDS_PAGE

// Always show the quit confirmation page
#define ALWAYS_SHOW_QUIT_CONFIRM_PAGE

// Complete the "Change Ghost Data" section (repurposed "Change Mission")
#define CHANGE_GHOST_DATA_SUPPORT

// Support changing settings in-race
#define INGAME_LICENSE_SETTINGS

// The channel section is repurposed into the Service Pack section
#define MORE_CHANNEL_PAGES

#define MISSION_MODE

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
#define EXTRA_QUIT_CONFIRM(section) Section_addPage(section, PAGE_ID_CONFIRM_QUIT)
#else
#define EXTRA_QUIT_CONFIRM(...)
#endif

#ifdef INGAME_LICENSE_SETTINGS
#define LICENSE_SETTINGS(section, prev) AddSettingsPage(section, prev)
// The license settings should *not* be active
#define LICENSE_ACTIVE(...)
#else
#define LICENSE_SETTINGS(...)
#define LICENSE_ACTIVE(...)
#endif

static void AddSettingsPage(Section *section, u32 prev) {
    LicenseSettingsPage *pSettings =
            (LicenseSettingsPage *)Section_createPage(PAGE_ID_LICENSE_RECORDS);
    pSettings->enableBackButton = false;
    pSettings->enableInstructionText = false;
    pSettings->replacementPage = prev;

    section->pages[PAGE_ID_LICENSE_RECORDS] = pSettings;
    Page_init((Page *)pSettings, PAGE_ID_LICENSE_RECORDS);
}

void my_Section_addPages(Section *section, u32 sectionId) {
    OSReport("&7DEBUG: Constructing section %u (0x%x)\n", sectionId, sectionId);
    switch (sectionId) {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        Section_addPage(section, 0);
        break;
    case 16:
        Section_addPage(section, 3);
        Section_addPage(section, 79);
        Section_addPage(section, 93);
        break;
    case 17:
    case 18:
        Section_addPage(section, 3);
        Section_addPage(section, 79);
        Section_addPage(section, 83);
        Section_addPage(section, 93);
        break;
    case 19:
        Section_addPage(section, 4);
        Section_addPage(section, 83);
        Section_addPage(section, 93);
        break;
    case 20:
        Section_addPage(section, 5);
        Section_addPage(section, 93);
        break;
    case 21:
        Section_addPage(section, 6);
        Section_addPage(section, 93);
        break;
    case 22:
        Section_addPage(section, 1);
        break;
    case 23:
    case 24:
        Section_addPage(section, 2);
        break;
    case 25:
        Section_addPage(section, 7);
        break;
    case 26:
        Section_addPage(section, 8);
        break;
    case 27:
        Section_addPage(section, 9);
        break;
    case 28:
        Section_addPage(section, 10);
        break;
    case 29:
        Section_addPage(section, 11);
        break;
    case SECTION_ID_GP:
        Section_addPage(section, 12);
        Section_addPage(section, PAGE_ID_GP_PAUSE_MENU);
        Section_addPage(section, 32);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_GP_PAUSE_MENU);
        break;
    case SECTION_ID_TIME_ATTACK:
        Section_addPage(section, 13);
        Section_addPage(section, PAGE_ID_TA_PAUSE_MENU);
        Section_addPage(section, 33);
        Section_addPage(section, 41);
        Section_addPage(section, 43);

        EXTRA_QUIT_CONFIRM(section);

        Section_addPage(section, 45);
        Section_addPage(section, 46);
        // Wipe controls
        Section_addPage(section, 58);
        Section_addPage(section, 167);

        // In-game settings
        // prev: PAGE_ID_TA_PAUSE_MENU
        LICENSE_SETTINGS(section, PAGE_ID_TA_PAUSE_MENU);
        break;
    case SECTION_ID_VS_1P:
        Section_addPage(section, 14);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        EXTRA_QUIT_CONFIRM(section);
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_VS_2P:
        Section_addPage(section, 15);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_VS_3P:
        Section_addPage(section, 16);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_VS_4P:
        Section_addPage(section, 17);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_TEAM_VS_1P:
        Section_addPage(section, 14);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        EXTRA_QUIT_CONFIRM(section);
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_TEAM_VS_2P:
        Section_addPage(section, 15);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_TEAM_VS_3P:
        Section_addPage(section, 16);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_TEAM_VS_4P:
        Section_addPage(section, 17);
        Section_addPage(section, PAGE_ID_VS_PAUSE_MENU);
        Section_addPage(section, 34);
        Section_addPage(section, 44);
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_VS_PAUSE_MENU);
        break;
    case SECTION_ID_BATTLE_1P:
        Section_addPage(section, 18);
        Section_addPage(section, PAGE_ID_BATTLE_PAUSE_MENU);
        Section_addPage(section, 35);
        Section_addPage(section, 36);
        EXTRA_QUIT_CONFIRM(section);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_BATTLE_PAUSE_MENU);
        break;
    case SECTION_ID_BATTLE_2P:
        Section_addPage(section, 19);
        Section_addPage(section, PAGE_ID_BATTLE_PAUSE_MENU);
        Section_addPage(section, 35);
        Section_addPage(section, 36);
        Section_addPage(section, 44);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_BATTLE_PAUSE_MENU);
        break;
    case SECTION_ID_BATTLE_3P:
        Section_addPage(section, 20);
        Section_addPage(section, PAGE_ID_BATTLE_PAUSE_MENU);
        Section_addPage(section, 35);
        Section_addPage(section, 36);
        Section_addPage(section, 44);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_BATTLE_PAUSE_MENU);
        break;
    case SECTION_ID_BATTLE_4P:
        Section_addPage(section, 21);
        Section_addPage(section, PAGE_ID_BATTLE_PAUSE_MENU);
        Section_addPage(section, 35);
        Section_addPage(section, 36);
        Section_addPage(section, 44);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_BATTLE_PAUSE_MENU);
        break;
    case SECTION_ID_MR_REPLAY:
        Section_addPage(section, 22);
        Section_addPage(section, PAGE_ID_MR_PAUSE_MENU);
        Section_addPage(section, 37);
        Section_addPage(section, 44);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_MR_PAUSE_MENU);

        // Mission Mode
        Section_addPage(section, 0x35);
        Section_addPage(section, 0x2A); // Bean's code loads 2A instead, which is Battle 3P menu... I assume that was a mistake and he meant this one
        break;
    case SECTION_ID_TOURNAMENT_REPLAY:
        Section_addPage(section, 22);
        Section_addPage(section, PAGE_ID_MR_PAUSE_MENU);
        Section_addPage(section, 38);
        Section_addPage(section, 42);
        Section_addPage(section, 53);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_MR_PAUSE_MENU);
        break;
    case SECTION_ID_GP_REPLAY:
        Section_addPage(section, 44);
        Section_addPage(section, 54);
        Section_addPage(section, PAGE_ID_GP_REPLAY_PAUSE_MENU);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_GP_REPLAY_PAUSE_MENU);
        break;
    case SECTION_ID_TT_REPLAY:
        Section_addPage(section, 33);
        Section_addPage(section, 43);
        EXTRA_QUIT_CONFIRM(section);
        Section_addPage(section, 55);
        Section_addPage(section, PAGE_ID_TA_REPLAY_PAUSE_MENU);
        Section_addPage(section, 58);
        Section_addPage(section, 167);
        LICENSE_SETTINGS(section, PAGE_ID_TA_REPLAY_PAUSE_MENU);
        break;
    case SECTION_ID_GHOST_TA:
        Section_addPage(section, 13);
        Section_addPage(section, PAGE_ID_TA_GHOST_PAUSE_MENU);
        Section_addPage(section, 29);
        Section_addPage(section, 30);
        Section_addPage(section, 39);
        Section_addPage(section, 40);
        Section_addPage(section, 44);
        Section_addPage(section, 45);
        Section_addPage(section, 46);
        Section_addPage(section, 58);
        LICENSE_SETTINGS(section, PAGE_ID_TA_GHOST_PAUSE_MENU);
        break;
    case SECTION_ID_GHOST_TA_ONLINE:
        Section_addPage(section, 13);
        Section_addPage(section, PAGE_ID_TA_GHOST_PAUSE_MENU);
        Section_addPage(section, 29);
        Section_addPage(section, 30);
        Section_addPage(section, 39);
        Section_addPage(section, 40);
        Section_addPage(section, 44);
        Section_addPage(section, 45);
        Section_addPage(section, 46);
        Section_addPage(section, 58);
        //
        Section_addPage(section, 136);
        LICENSE_SETTINGS(section, PAGE_ID_TA_GHOST_PAUSE_MENU);
        break;
    case SECTION_ID_GHOST_REPLAY_CHANNEL:
    case SECTION_ID_GHOST_REPLAY_DL:
    case SECTION_ID_GHOST_REPLAY:
        Section_addPage(section, PAGE_ID_GHOST_REPLAY_PAUSE_MENU);
        Section_addPage(section, 44);
        Section_addPage(section, 55);
        Section_addPage(section, 58);
        Section_addPage(section, 167);
        LICENSE_SETTINGS(section, PAGE_ID_GHOST_REPLAY_PAUSE_MENU);
        break;
    case 53:
    case 54:
    case 55:
    case 56:
        Section_addPage(section, 59);
        Section_addPage(section, 60);
        break;
    case 57:
    case 58:
        Section_addPage(section, 61);
        break;
    case 59:
    case 60:
        Section_addPage(section, 61);
        Section_addPage(section, 62);
        break;
    case 61:
    case 62:
        Section_addPage(section, 63);
        Section_addPage(section, 79);
        Section_addPage(section, 81);
        Section_addPage(section, 83);
        Section_addPage(section, 127);
        break;
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
        Section_addPage(section, 77);
        Section_addPage(section, 82);
        Section_addPage(section, 84);
        Section_addPage(section, 85);
        Section_addPage(section, 86);
        Section_addPage(section, 87);
        Section_addPage(section, 88);
        Section_addPage(section, 89);
        Section_addPage(section, 90);
        Section_addPage(section, 94);
        Section_addPage(section, 101);
        break;
    case 68:
        Section_addPage(section, 58);
        Section_addPage(section, 95);
        break;
    case 69:
    case 70:
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 93);
        Section_addPage(section, 94);
        Section_addPage(section, 96);
        Section_addPage(section, 102);
        break;
    case 71:
        Section_addPage(section, 78);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 93);
        Section_addPage(section, 94);
        Section_addPage(section, 96);
        Section_addPage(section, 102);
        Section_addPage(section, 103);
        Section_addPage(section, 104);

        Section_addPage(section, PAGE_ID_LICENSE_RECORDS);
#ifndef ONLY_ONE_RECORDS_PAGE
        Section_addPage(section, PAGE_ID_LICENSE_RECORDS_FAVORITES);
        Section_addPage(section, PAGE_ID_LICENSE_RECORDS_FRIENDS);
        Section_addPage(section, PAGE_ID_LICENSE_RECORDS_WFC);
        Section_addPage(section, PAGE_ID_LICENSE_RECORDS_OTHER);
#endif
        break;
    case 72:
    case 73:
    case 74:
    case 77:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 79);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 83);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 105);
        Section_addPage(section, 106);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 110);
        Section_addPage(section, 111);
        Section_addPage(section, 112);
        Section_addPage(section, 113);
        Section_addPage(section, 114);
        Section_addPage(section, 115);
        Section_addPage(section, 116);
        Section_addPage(section, 117);
        Section_addPage(section, 118);
        Section_addPage(section, 119);
        Section_addPage(section, 120);
        Section_addPage(section, 121);
        Section_addPage(section, 127);
        Section_addPage(section, 167);

        // Mission Mode
        missionModeFunc1();
        break;
    case 75:
        Section_addPage(section, 75);
        Section_addPage(section, 78);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 83);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 110);
        Section_addPage(section, 111);
        Section_addPage(section, 127);
        Section_addPage(section, 167);
        break;
    case 76:
        Section_addPage(section, 75);
        Section_addPage(section, 78);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 83);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 120);
        Section_addPage(section, 121);
        Section_addPage(section, 127);
        Section_addPage(section, 167);
        break;
    case 78:
    case 79:
    case 80:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 127);
        Section_addPage(section, 167);
        break;
    case 81:
        Section_addPage(section, 79);
        Section_addPage(section, 81);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 167);
        Section_addPage(section, 182);
        break;
    case 82:
    case 83:
        Section_addPage(section, 79);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 167);
        Section_addPage(section, 177);
        Section_addPage(section, 178);
        break;
    case 84:
        Section_addPage(section, 75);
        Section_addPage(section, 76);
        Section_addPage(section, 77);
        Section_addPage(section, 82);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 96);
        Section_addPage(section, 97);
        Section_addPage(section, 98);
        Section_addPage(section, 99);
        Section_addPage(section, 100);
        Section_addPage(section, 107);
        Section_addPage(section, 110);
        Section_addPage(section, 111);
        Section_addPage(section, 114);
        Section_addPage(section, 115);
        Section_addPage(section, 116);
        Section_addPage(section, 117);
        Section_addPage(section, 119);
        Section_addPage(section, 120);
        Section_addPage(section, 121);
        Section_addPage(section, 127);
        Section_addPage(section, 128);
        Section_addPage(section, 129);
        Section_addPage(section, 130);
        Section_addPage(section, 131);
        break;
    case 85:
    case 86:
    case 87:
        Section_addPage(section, 76);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 118);
        Section_addPage(section, 127);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 135);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 139);
        Section_addPage(section, 140);
        Section_addPage(section, 141);
        Section_addPage(section, 143);
        Section_addPage(section, 149);
        Section_addPage(section, 150);
        Section_addPage(section, 151);
        Section_addPage(section, 152);
        Section_addPage(section, 153);
        Section_addPage(section, 155);
        Section_addPage(section, 156);
        Section_addPage(section, 157);
        Section_addPage(section, 158);
        Section_addPage(section, 165);
        Section_addPage(section, 166);
        Section_addPage(section, 167);
        break;
    case 88:
    case 94:
    case 96:
    case 97:
    case 100:
    case 101:
        Section_addPage(section, 78);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 110);
        Section_addPage(section, 111);
        Section_addPage(section, 127);
        Section_addPage(section, 136);
        Section_addPage(section, 144);
        Section_addPage(section, 145);
        Section_addPage(section, 146);
        break;
    case 89:
    case 95:
    case 98:
    case 99:
    case 102:
    case 103:
        Section_addPage(section, 78);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 92);
        Section_addPage(section, 94);
        Section_addPage(section, 120);
        Section_addPage(section, 121);
        Section_addPage(section, 127);
        Section_addPage(section, 136);
        Section_addPage(section, 144);
        Section_addPage(section, 145);
        Section_addPage(section, 146);
        break;
    case 90:
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 96);
        Section_addPage(section, 97);
        Section_addPage(section, 98);
        Section_addPage(section, 99);
        Section_addPage(section, 100);
        Section_addPage(section, 138);
        break;
    case 91:
    case 92:
    case 93:
        Section_addPage(section, 76);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 118);
        Section_addPage(section, 127);
        Section_addPage(section, 129);
        Section_addPage(section, 130);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 135);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 139);
        Section_addPage(section, 140);
        Section_addPage(section, 141);
        Section_addPage(section, 143);
        Section_addPage(section, 149);
        Section_addPage(section, 150);
        Section_addPage(section, 151);
        Section_addPage(section, 152);
        Section_addPage(section, 153);
        Section_addPage(section, 155);
        Section_addPage(section, 156);
        Section_addPage(section, 157);
        Section_addPage(section, 158);
        Section_addPage(section, 165);
        Section_addPage(section, 166);
        Section_addPage(section, 167);
        break;
    case 104:
        Section_addPage(section, 49);
        Section_addPage(section, 58);
        Section_addPage(section, 64);
        Section_addPage(section, 69);
        Section_addPage(section, 70);
        Section_addPage(section, 71);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 105:
        Section_addPage(section, 49);
        Section_addPage(section, 58);
        Section_addPage(section, 65);
        Section_addPage(section, 69);
        Section_addPage(section, 70);
        Section_addPage(section, 71);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 106:
    case 107:
        Section_addPage(section, 58);
        Section_addPage(section, 72);
        Section_addPage(section, 73);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 147);
        Section_addPage(section, 148);
        break;
    case 108:
        Section_addPage(section, 18);
        Section_addPage(section, 49);
        Section_addPage(section, 51);
        Section_addPage(section, 58);
        Section_addPage(section, 69);
        Section_addPage(section, 70);
        Section_addPage(section, 71);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 109:
        Section_addPage(section, 19);
        Section_addPage(section, 49);
        Section_addPage(section, 51);
        Section_addPage(section, 58);
        Section_addPage(section, 69);
        Section_addPage(section, 70);
        Section_addPage(section, 71);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 110:
    case 111:
        Section_addPage(section, 51);
        Section_addPage(section, 58);
        Section_addPage(section, 72);
        Section_addPage(section, 74);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 147);
        Section_addPage(section, 148);
        break;
    case 112:
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        Section_addPage(section, 66);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 113:
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        Section_addPage(section, 66);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 114:
    case 115:
        Section_addPage(section, 18);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 116:
        Section_addPage(section, 47);
        Section_addPage(section, 48);
        Section_addPage(section, 58);
        Section_addPage(section, 67);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 117:
        Section_addPage(section, 47);
        Section_addPage(section, 50);
        Section_addPage(section, 58);
        Section_addPage(section, 67);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 118:
    case 119:
        Section_addPage(section, 19);
        Section_addPage(section, 51);
        Section_addPage(section, 52);
        Section_addPage(section, 58);
        Section_addPage(section, 68);
        Section_addPage(section, 72);
        Section_addPage(section, 81);
        Section_addPage(section, 136);
        Section_addPage(section, 148);
        break;
    case 120:
    case 121:
        Section_addPage(section, 93);
        Section_addPage(section, 137);
        break;
    case 122:
    case 123:
    case 124:
#ifndef MORE_CHANNEL_PAGES
        Section_addPage(section, 77);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
#endif
        Section_addPage(section, 94);
        Section_addPage(section, 162);
#ifndef MORE_CHANNEL_PAGES
        Section_addPage(section, 163);
        Section_addPage(section, 164);
#endif
#ifdef MORE_CHANNEL_PAGES
        Section_addPage(section, 194);
        Section_addPage(section, 195);
        Section_addPage(section, 199);
        Section_addPage(section, 200);
        Section_addPage(section, 201);
        Section_addPage(section, 202);
#endif
        break;
    case 125:
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 110);
        Section_addPage(section, 111);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 149);
        Section_addPage(section, 167);
        Section_addPage(section, 168);
        Section_addPage(section, 170);
        Section_addPage(section, 171);
        Section_addPage(section, 172);
        Section_addPage(section, 173);
        Section_addPage(section, 174);
        Section_addPage(section, 175);
        break;
    case 126:
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 149);
        Section_addPage(section, 169);
        Section_addPage(section, 170);
        Section_addPage(section, 171);
        Section_addPage(section, 174);
        Section_addPage(section, 175);
        Section_addPage(section, 176);
        break;
    case 127:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 81);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 127);
        Section_addPage(section, 159);
        Section_addPage(section, 160);
        Section_addPage(section, 161);
        break;
    case 128:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 127);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 159);
        Section_addPage(section, 160);
        Section_addPage(section, 161);
        break;
    case 129:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 127);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 160);
        Section_addPage(section, 161);
        break;
    case 130:
        Section_addPage(section, 77);
        Section_addPage(section, 79);
        Section_addPage(section, 81);
        Section_addPage(section, 94);
        Section_addPage(section, 167);
        Section_addPage(section, 179);
        Section_addPage(section, 180);
        Section_addPage(section, 181);
        break;
    case 131:
        Section_addPage(section, 76);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 118);
        Section_addPage(section, 127);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 135);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 142);
        Section_addPage(section, 143);
        Section_addPage(section, 149);
        Section_addPage(section, 150);
        Section_addPage(section, 151);
        Section_addPage(section, 152);
        Section_addPage(section, 153);
        Section_addPage(section, 155);
        Section_addPage(section, 156);
        Section_addPage(section, 157);
        Section_addPage(section, 158);
        Section_addPage(section, 165);
        Section_addPage(section, 166);
        Section_addPage(section, 167);
        break;
    case 132:
        Section_addPage(section, 76);
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 118);
        Section_addPage(section, 127);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 135);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 142);
        Section_addPage(section, 143);
        Section_addPage(section, 149);
        Section_addPage(section, 150);
        Section_addPage(section, 151);
        Section_addPage(section, 152);
        Section_addPage(section, 153);
        Section_addPage(section, 154);
        Section_addPage(section, 155);
        Section_addPage(section, 156);
        Section_addPage(section, 157);
        Section_addPage(section, 158);
        Section_addPage(section, 165);
        Section_addPage(section, 166);
        Section_addPage(section, 167);
        break;
    case 133:
    case 134:
        Section_addPage(section, 75);
        Section_addPage(section, 77);
        Section_addPage(section, 81);
        Section_addPage(section, 107);
        Section_addPage(section, 108);
        Section_addPage(section, 109);
        Section_addPage(section, 127);
        Section_addPage(section, 183);
        Section_addPage(section, 184);
        Section_addPage(section, 187);
        break;
    case 135:
        Section_addPage(section, 78);
        Section_addPage(section, 79);
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 94);
        Section_addPage(section, 132);
        Section_addPage(section, 133);
        Section_addPage(section, 134);
        Section_addPage(section, 136);
        Section_addPage(section, 137);
        Section_addPage(section, 185);
        break;
    case 136:
    case 137:
        Section_addPage(section, 91);
        Section_addPage(section, 186);
        break;
    case 138:
        Section_addPage(section, 79);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 189);
        Section_addPage(section, 190);
        Section_addPage(section, 191);
        break;
    case 139:
        Section_addPage(section, 80);
        Section_addPage(section, 81);
        Section_addPage(section, 82);
        Section_addPage(section, 94);
        Section_addPage(section, 188);
        break;
    case 140:
        Section_addPage(section, 77);
        Section_addPage(section, 78);
        Section_addPage(section, 93);
        Section_addPage(section, 192);
        Section_addPage(section, 193);
        Section_addPage(section, 194);
        Section_addPage(section, 195);
        Section_addPage(section, 196);
        Section_addPage(section, 197);
        Section_addPage(section, 198);
        Section_addPage(section, 199);
        Section_addPage(section, 200);
        Section_addPage(section, 201);
        Section_addPage(section, 202);
        Section_addPage(section, 203);
        Section_addPage(section, 204);
        break;
    case 141:
        Section_addPage(section, 93);
        Section_addPage(section, 202);
        Section_addPage(section, 203);
        Section_addPage(section, 204);
        break;
    case 142:
        Section_addPage(section, 93);
        Section_addPage(section, 197);
        Section_addPage(section, 204);
        break;
    case 143:
        Section_addPage(section, 78);
        Section_addPage(section, 93);
        Section_addPage(section, 194);
        Section_addPage(section, 195);
        Section_addPage(section, 198);
        Section_addPage(section, 199);
        Section_addPage(section, 200);
        Section_addPage(section, 201);
        Section_addPage(section, 204);
        break;
    case 144:
    case 145:
    case 146:
    case 147:
        Section_addPage(section, 91);
        Section_addPage(section, 94);
        Section_addPage(section, 127);
        Section_addPage(section, 205);
        break;
    case 148:
        Section_addPage(section, 122);
        Section_addPage(section, 123);
        Section_addPage(section, 124);
        Section_addPage(section, 125);
        Section_addPage(section, 126);
        break;
    default:
        return;
    }
}

void my_Section_addActivePages(Section *section, u32 sectionId) {
    switch (sectionId) {
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        Section_addActivePage(section, 0);
        break;
    case 16:
    case 17:
    case 18:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 3);
        break;
    case 19:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 4);
        break;
    case 20:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 5);
        break;
    case 21:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 6);
        break;
    case 22:
        Section_addActivePage(section, 1);
        break;
    case 23:
    case 24:
        Section_addActivePage(section, 2);
        break;
    case 25:
        Section_addActivePage(section, 7);
        break;
    case 26:
        Section_addActivePage(section, 8);
        break;
    case 27:
        Section_addActivePage(section, 9);
        break;
    case 28:
        Section_addActivePage(section, 10);
        break;
    case 29:
        Section_addActivePage(section, 11);
        break;
    case SECTION_ID_GP:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 12);
        break;
    case SECTION_ID_TIME_ATTACK:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 13);
        break;
    case SECTION_ID_VS_1P:
    case SECTION_ID_TEAM_VS_1P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 14);
        break;
    case SECTION_ID_VS_2P:
    case SECTION_ID_TEAM_VS_2P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 15);
        break;
    case SECTION_ID_VS_3P:
    case SECTION_ID_TEAM_VS_3P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 16);
        break;
    case SECTION_ID_VS_4P:
    case SECTION_ID_TEAM_VS_4P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 17);
        break;
    case SECTION_ID_BATTLE_1P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 18);
        break;
    case SECTION_ID_BATTLE_2P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 19);
        break;
    case SECTION_ID_BATTLE_3P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 20);
        break;
    case SECTION_ID_BATTLE_4P:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 21);
        break;
    case SECTION_ID_MR_REPLAY:
    case SECTION_ID_TOURNAMENT_REPLAY:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 22);
        break;
    case SECTION_ID_GP_REPLAY:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 54);
        break;
    case SECTION_ID_TT_REPLAY:
    case SECTION_ID_GHOST_REPLAY_CHANNEL:
    case SECTION_ID_GHOST_REPLAY_DL:
    case SECTION_ID_GHOST_REPLAY:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 55);
        break;
    case SECTION_ID_GHOST_TA:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 13);
        break;
    case SECTION_ID_GHOST_TA_ONLINE:
        LICENSE_ACTIVE(section);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 13);
        break;
    case 53:
    case 54:
    case 55:
    case 56:
        Section_addActivePage(section, 59);
        break;
    case 57:
    case 58:
        Section_addActivePage(section, 61);
        break;
    case 59:
    case 60:
        Section_addActivePage(section, 62);
        Section_addActivePage(section, 61);
        break;
    case 61:
    case 62:
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 63);
        break;
    case 63:
    case 64:
        Section_addActivePage(section, 84);
        Section_addActivePage(section, 88);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 87);
        break;
    case 65:
        Section_addActivePage(section, 84);
        Section_addActivePage(section, 88);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 90);
        break;
    case 66:
    case 67:
        Section_addActivePage(section, 84);
        Section_addActivePage(section, 88);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 101);
        break;
    case 68:
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 95);
        break;
    case 69:
    case 70:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 96);
        break;
    case 71:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 103);
        break;
    case 72:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 105);
        break;
    case 73:
    case 78:
    case 79:
    case 80:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 107);
        break;
    case 74:
    case 75:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 110);
        break;
    case 76:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 120);
        break;
    case 77:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 127);
        OSReport("HERE IT IS: %i", section->id);
#ifdef CHANGE_GHOST_DATA_SUPPORT
        Section_addActivePage(section, 112);
#endif
        // Mission Mode
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 122);
        break;
    case 81:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 182);
        break;
    case 82:
    case 83:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 177);
        break;
    case 84:
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 97);
        break;
    case 85:
    case 91:
    case 131:
    case 132:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 149);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 132);
        break;
    case 86:
    case 92:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 149);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 155);
        break;
    case 87:
    case 93:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 149);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 150);
        break;
    case 88:
    case 89:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 92);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 144);
        break;
    case 90:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 97);
        break;
    case 104:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 64);
        break;
    case 105:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 65);
        break;
    case 106:
    case 107:
    case 110:
    case 111:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 147);
        break;
    case 108:
    case 114:
    case 115:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 18);
        break;
    case 109:
    case 118:
    case 119:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 19);
        break;
    case 112:
    case 113:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 66);
        break;
    case 116:
    case 117:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 148);
        Section_addActivePage(section, 58);
        Section_addActivePage(section, 67);
        break;
    case 120:
    case 121:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 137);
        break;
    case 122:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 162);
        break;
    case 123:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 163);
        break;
    case 124:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 164);
        break;
    case 125:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 170);
        Section_addActivePage(section, 171);
        Section_addActivePage(section, 149);
        Section_addActivePage(section, 168);
        break;
    case 126:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 170);
        Section_addActivePage(section, 171);
        Section_addActivePage(section, 149);
        Section_addActivePage(section, 176);
        break;
    case 127:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 159);
        break;
    case 128:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 159);
        break;
    case 129:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 132);
        break;
    case 130:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 167);
        Section_addActivePage(section, 179);
        break;
    case 133:
        Section_addActivePage(section, 187);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 184);
        break;
    case 134:
        Section_addActivePage(section, 187);
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 183);
        break;
    case 135:
        Section_addActivePage(section, 136);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 132);
        break;
    case 136:
    case 137:
        Section_addActivePage(section, 91);
        Section_addActivePage(section, 186);
        break;
    case 138:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 189);
        break;
    case 139:
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 188);
        break;
    case 140:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 204);
        Section_addActivePage(section, 192);
        break;
    case 141:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 204);
        Section_addActivePage(section, 202);
        break;
    case 142:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 204);
        Section_addActivePage(section, 197);
        break;
    case 143:
        Section_addActivePage(section, 93);
        Section_addActivePage(section, 204);
        Section_addActivePage(section, 198);
        break;
    case 144:
    case 145:
    case 146:
    case 147:
        Section_addActivePage(section, 127);
        Section_addActivePage(section, 91);
        Section_addActivePage(section, 94);
        Section_addActivePage(section, 205);
        break;
    default:
        return;
    }
}

PATCH_B(Section_addPages, my_Section_addPages);
PATCH_B(Section_addActivePages, my_Section_addActivePages);
