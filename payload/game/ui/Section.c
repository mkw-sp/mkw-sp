#include "Section.h"

#include "LicenseSelectPage.h"
#include "LicenseSettingsPage.h"
#include "TimeAttackGhostListPage.h"
#include "TimeAttackRulesPage.h"

PATCH_S16(Section_createPage, 0x92e, sizeof(LicenseSelectPage));
PATCH_S16(Section_createPage, 0x9a6, sizeof(TimeAttackRulesPage));
PATCH_S16(Section_createPage, 0xa4e, sizeof(TimeAttackGhostListPage));
PATCH_S16(Section_createPage, 0x12d6, sizeof(LicenseSettingsPage));

// The game has 5 pages for the records, we only need 1 for the settings. Disable the 4
// others.
#define ONLY_ONE_RECORDS_PAGE

// Always show the quit confirmation page
#define ALWAYS_SHOW_QUIT_CONFIRM_PAGE

// Complete the "Change Ghost Data" section (repurposed "Change Mission")
#define CHANGE_GHOST_DATA_SUPPORT

void my_Section_addPages(Section *pSection, u32 sectionId) {
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
        Section_addPage(pSection, 0);
        break;
    case 16:
        Section_addPage(pSection, 3);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 93);
        break;
    case 17:
    case 18:
        Section_addPage(pSection, 3);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 93);
        break;
    case 19:
        Section_addPage(pSection, 4);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 93);
        break;
    case 20:
        Section_addPage(pSection, 5);
        Section_addPage(pSection, 93);
        break;
    case 21:
        Section_addPage(pSection, 6);
        Section_addPage(pSection, 93);
        break;
    case 22:
        Section_addPage(pSection, 1);
        break;
    case 23:
    case 24:
        Section_addPage(pSection, 2);
        break;
    case 25:
        Section_addPage(pSection, 7);
        break;
    case 26:
        Section_addPage(pSection, 8);
        break;
    case 27:
        Section_addPage(pSection, 9);
        break;
    case 28:
        Section_addPage(pSection, 10);
        break;
    case 29:
        Section_addPage(pSection, 11);
        break;
    case 30:
        Section_addPage(pSection, 12);
        Section_addPage(pSection, 23);
        Section_addPage(pSection, 32);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        break;
    case 31:
        Section_addPage(pSection, 13);
        Section_addPage(pSection, 25);
        Section_addPage(pSection, 33);
        Section_addPage(pSection, 41);
        Section_addPage(pSection, 43);

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
        Section_addPage(pSection, 44);
#endif

        Section_addPage(pSection, 45);
        Section_addPage(pSection, 46);
        // Wipe controls
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 167);
        break;
    case 32:
        Section_addPage(pSection, 14);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
        Section_addPage(pSection, 44);
#endif

        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        break;
    case 33:
        Section_addPage(pSection, 15);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        break;
    case 34:
        Section_addPage(pSection, 16);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        break;
    case 35:
        Section_addPage(pSection, 17);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        break;
    case 36:
        Section_addPage(pSection, 14);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
        Section_addPage(pSection, 44);
#endif

        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        break;
    case 37:
        Section_addPage(pSection, 15);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        break;
    case 38:
        Section_addPage(pSection, 16);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        break;
    case 39:
        Section_addPage(pSection, 17);
        Section_addPage(pSection, 24);
        Section_addPage(pSection, 34);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        break;
    case 40:
        Section_addPage(pSection, 18);
        Section_addPage(pSection, 26);
        Section_addPage(pSection, 35);
        Section_addPage(pSection, 36);

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
        Section_addPage(pSection, 44);
#endif

        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        break;
    case 41:
        Section_addPage(pSection, 19);
        Section_addPage(pSection, 26);
        Section_addPage(pSection, 35);
        Section_addPage(pSection, 36);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        break;
    case 42:
        Section_addPage(pSection, 20);
        Section_addPage(pSection, 26);
        Section_addPage(pSection, 35);
        Section_addPage(pSection, 36);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        break;
    case 43:
        Section_addPage(pSection, 21);
        Section_addPage(pSection, 26);
        Section_addPage(pSection, 35);
        Section_addPage(pSection, 36);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        break;
    case 44:
        Section_addPage(pSection, 22);
        Section_addPage(pSection, 27);
        Section_addPage(pSection, 37);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 58);
        break;
    case 45:
        Section_addPage(pSection, 22);
        Section_addPage(pSection, 27);
        Section_addPage(pSection, 38);
        Section_addPage(pSection, 42);
        Section_addPage(pSection, 53);
        Section_addPage(pSection, 58);
        break;
    case 46:
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 54);
        Section_addPage(pSection, 56);
        Section_addPage(pSection, 58);
        break;
    case 47:
        Section_addPage(pSection, 33);
        Section_addPage(pSection, 43);

#ifdef ALWAYS_SHOW_QUIT_CONFIRM_PAGE
        Section_addPage(pSection, 44);
#endif

        Section_addPage(pSection, 55);
        Section_addPage(pSection, 57);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 167);
        break;
    case 48:
        Section_addPage(pSection, 13);
        Section_addPage(pSection, 28);
        Section_addPage(pSection, 29);
        Section_addPage(pSection, 30);
        Section_addPage(pSection, 39);
        Section_addPage(pSection, 40);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 45);
        Section_addPage(pSection, 46);
        Section_addPage(pSection, 58);
        break;
    case 49:
        Section_addPage(pSection, 13);
        Section_addPage(pSection, 28);
        Section_addPage(pSection, 29);
        Section_addPage(pSection, 30);
        Section_addPage(pSection, 39);
        Section_addPage(pSection, 40);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 45);
        Section_addPage(pSection, 46);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 136);
        break;
    case 50:
    case 51:
    case 52:
        Section_addPage(pSection, 31);
        Section_addPage(pSection, 44);
        Section_addPage(pSection, 55);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 167);
        break;
    case 53:
    case 54:
    case 55:
    case 56:
        Section_addPage(pSection, 59);
        Section_addPage(pSection, 60);
        break;
    case 57:
    case 58:
        Section_addPage(pSection, 61);
        break;
    case 59:
    case 60:
        Section_addPage(pSection, 61);
        Section_addPage(pSection, 62);
        break;
    case 61:
    case 62:
        Section_addPage(pSection, 63);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 127);
        break;
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 84);
        Section_addPage(pSection, 85);
        Section_addPage(pSection, 86);
        Section_addPage(pSection, 87);
        Section_addPage(pSection, 88);
        Section_addPage(pSection, 89);
        Section_addPage(pSection, 90);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 101);
        break;
    case 68:
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 95);
        break;
    case 69:
    case 70:
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 96);
        Section_addPage(pSection, 102);
        break;
    case 71:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 96);
        Section_addPage(pSection, 102);
        Section_addPage(pSection, 103);
        Section_addPage(pSection, 104);
        Section_addPage(pSection, 206);
#ifndef ONLY_ONE_RECORDS_PAGE
        Section_addPage(pSection, 207);
        Section_addPage(pSection, 208);
        Section_addPage(pSection, 209);
        Section_addPage(pSection, 210);
#endif
        break;
    case 72:
    case 73:
    case 74:
    case 77:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 105);
        Section_addPage(pSection, 106);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 110);
        Section_addPage(pSection, 111);
        Section_addPage(pSection, 112);
        Section_addPage(pSection, 113);
        Section_addPage(pSection, 114);
        Section_addPage(pSection, 115);
        Section_addPage(pSection, 116);
        Section_addPage(pSection, 117);
        Section_addPage(pSection, 118);
        Section_addPage(pSection, 119);
        Section_addPage(pSection, 120);
        Section_addPage(pSection, 121);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 167);
        break;
    case 75:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 110);
        Section_addPage(pSection, 111);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 167);
        break;
    case 76:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 83);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 120);
        Section_addPage(pSection, 121);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 167);
        break;
    case 78:
    case 79:
    case 80:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 167);
        break;
    case 81:
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 167);
        Section_addPage(pSection, 182);
        break;
    case 82:
    case 83:
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 167);
        Section_addPage(pSection, 177);
        Section_addPage(pSection, 178);
        break;
    case 84:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 76);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 96);
        Section_addPage(pSection, 97);
        Section_addPage(pSection, 98);
        Section_addPage(pSection, 99);
        Section_addPage(pSection, 100);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 110);
        Section_addPage(pSection, 111);
        Section_addPage(pSection, 114);
        Section_addPage(pSection, 115);
        Section_addPage(pSection, 116);
        Section_addPage(pSection, 117);
        Section_addPage(pSection, 119);
        Section_addPage(pSection, 120);
        Section_addPage(pSection, 121);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 128);
        Section_addPage(pSection, 129);
        Section_addPage(pSection, 130);
        Section_addPage(pSection, 131);
        break;
    case 85:
    case 86:
    case 87:
        Section_addPage(pSection, 76);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 118);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 135);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 139);
        Section_addPage(pSection, 140);
        Section_addPage(pSection, 141);
        Section_addPage(pSection, 143);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 150);
        Section_addPage(pSection, 151);
        Section_addPage(pSection, 152);
        Section_addPage(pSection, 153);
        Section_addPage(pSection, 155);
        Section_addPage(pSection, 156);
        Section_addPage(pSection, 157);
        Section_addPage(pSection, 158);
        Section_addPage(pSection, 165);
        Section_addPage(pSection, 166);
        Section_addPage(pSection, 167);
        break;
    case 88:
    case 94:
    case 96:
    case 97:
    case 100:
    case 101:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 110);
        Section_addPage(pSection, 111);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 144);
        Section_addPage(pSection, 145);
        Section_addPage(pSection, 146);
        break;
    case 89:
    case 95:
    case 98:
    case 99:
    case 102:
    case 103:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 92);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 120);
        Section_addPage(pSection, 121);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 144);
        Section_addPage(pSection, 145);
        Section_addPage(pSection, 146);
        break;
    case 90:
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 96);
        Section_addPage(pSection, 97);
        Section_addPage(pSection, 98);
        Section_addPage(pSection, 99);
        Section_addPage(pSection, 100);
        Section_addPage(pSection, 138);
        break;
    case 91:
    case 92:
    case 93:
        Section_addPage(pSection, 76);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 118);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 129);
        Section_addPage(pSection, 130);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 135);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 139);
        Section_addPage(pSection, 140);
        Section_addPage(pSection, 141);
        Section_addPage(pSection, 143);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 150);
        Section_addPage(pSection, 151);
        Section_addPage(pSection, 152);
        Section_addPage(pSection, 153);
        Section_addPage(pSection, 155);
        Section_addPage(pSection, 156);
        Section_addPage(pSection, 157);
        Section_addPage(pSection, 158);
        Section_addPage(pSection, 165);
        Section_addPage(pSection, 166);
        Section_addPage(pSection, 167);
        break;
    case 104:
        Section_addPage(pSection, 49);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 64);
        Section_addPage(pSection, 69);
        Section_addPage(pSection, 70);
        Section_addPage(pSection, 71);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 105:
        Section_addPage(pSection, 49);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 65);
        Section_addPage(pSection, 69);
        Section_addPage(pSection, 70);
        Section_addPage(pSection, 71);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 106:
    case 107:
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 73);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 147);
        Section_addPage(pSection, 148);
        break;
    case 108:
        Section_addPage(pSection, 18);
        Section_addPage(pSection, 49);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 69);
        Section_addPage(pSection, 70);
        Section_addPage(pSection, 71);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 109:
        Section_addPage(pSection, 19);
        Section_addPage(pSection, 49);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 69);
        Section_addPage(pSection, 70);
        Section_addPage(pSection, 71);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 110:
    case 111:
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 74);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 147);
        Section_addPage(pSection, 148);
        break;
    case 112:
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 66);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 113:
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 66);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 114:
    case 115:
        Section_addPage(pSection, 18);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 116:
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 48);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 67);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 117:
        Section_addPage(pSection, 47);
        Section_addPage(pSection, 50);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 67);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 118:
    case 119:
        Section_addPage(pSection, 19);
        Section_addPage(pSection, 51);
        Section_addPage(pSection, 52);
        Section_addPage(pSection, 58);
        Section_addPage(pSection, 68);
        Section_addPage(pSection, 72);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 148);
        break;
    case 120:
    case 121:
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 137);
        break;
    case 122:
    case 123:
    case 124:
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 162);
        Section_addPage(pSection, 163);
        Section_addPage(pSection, 164);
        break;
    case 125:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 110);
        Section_addPage(pSection, 111);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 167);
        Section_addPage(pSection, 168);
        Section_addPage(pSection, 170);
        Section_addPage(pSection, 171);
        Section_addPage(pSection, 172);
        Section_addPage(pSection, 173);
        Section_addPage(pSection, 174);
        Section_addPage(pSection, 175);
        break;
    case 126:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 169);
        Section_addPage(pSection, 170);
        Section_addPage(pSection, 171);
        Section_addPage(pSection, 174);
        Section_addPage(pSection, 175);
        Section_addPage(pSection, 176);
        break;
    case 127:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 159);
        Section_addPage(pSection, 160);
        Section_addPage(pSection, 161);
        break;
    case 128:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 159);
        Section_addPage(pSection, 160);
        Section_addPage(pSection, 161);
        break;
    case 129:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 160);
        Section_addPage(pSection, 161);
        break;
    case 130:
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 167);
        Section_addPage(pSection, 179);
        Section_addPage(pSection, 180);
        Section_addPage(pSection, 181);
        break;
    case 131:
        Section_addPage(pSection, 76);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 118);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 135);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 142);
        Section_addPage(pSection, 143);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 150);
        Section_addPage(pSection, 151);
        Section_addPage(pSection, 152);
        Section_addPage(pSection, 153);
        Section_addPage(pSection, 155);
        Section_addPage(pSection, 156);
        Section_addPage(pSection, 157);
        Section_addPage(pSection, 158);
        Section_addPage(pSection, 165);
        Section_addPage(pSection, 166);
        Section_addPage(pSection, 167);
        break;
    case 132:
        Section_addPage(pSection, 76);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 118);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 135);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 142);
        Section_addPage(pSection, 143);
        Section_addPage(pSection, 149);
        Section_addPage(pSection, 150);
        Section_addPage(pSection, 151);
        Section_addPage(pSection, 152);
        Section_addPage(pSection, 153);
        Section_addPage(pSection, 154);
        Section_addPage(pSection, 155);
        Section_addPage(pSection, 156);
        Section_addPage(pSection, 157);
        Section_addPage(pSection, 158);
        Section_addPage(pSection, 165);
        Section_addPage(pSection, 166);
        Section_addPage(pSection, 167);
        break;
    case 133:
    case 134:
        Section_addPage(pSection, 75);
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 107);
        Section_addPage(pSection, 108);
        Section_addPage(pSection, 109);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 183);
        Section_addPage(pSection, 184);
        Section_addPage(pSection, 187);
        break;
    case 135:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 132);
        Section_addPage(pSection, 133);
        Section_addPage(pSection, 134);
        Section_addPage(pSection, 136);
        Section_addPage(pSection, 137);
        Section_addPage(pSection, 185);
        break;
    case 136:
    case 137:
        Section_addPage(pSection, 91);
        Section_addPage(pSection, 186);
        break;
    case 138:
        Section_addPage(pSection, 79);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 189);
        Section_addPage(pSection, 190);
        Section_addPage(pSection, 191);
        break;
    case 139:
        Section_addPage(pSection, 80);
        Section_addPage(pSection, 81);
        Section_addPage(pSection, 82);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 188);
        break;
    case 140:
        Section_addPage(pSection, 77);
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 192);
        Section_addPage(pSection, 193);
        Section_addPage(pSection, 194);
        Section_addPage(pSection, 195);
        Section_addPage(pSection, 196);
        Section_addPage(pSection, 197);
        Section_addPage(pSection, 198);
        Section_addPage(pSection, 199);
        Section_addPage(pSection, 200);
        Section_addPage(pSection, 201);
        Section_addPage(pSection, 202);
        Section_addPage(pSection, 203);
        Section_addPage(pSection, 204);
        break;
    case 141:
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 202);
        Section_addPage(pSection, 203);
        Section_addPage(pSection, 204);
        break;
    case 142:
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 197);
        Section_addPage(pSection, 204);
        break;
    case 143:
        Section_addPage(pSection, 78);
        Section_addPage(pSection, 93);
        Section_addPage(pSection, 194);
        Section_addPage(pSection, 195);
        Section_addPage(pSection, 198);
        Section_addPage(pSection, 199);
        Section_addPage(pSection, 200);
        Section_addPage(pSection, 201);
        Section_addPage(pSection, 204);
        break;
    case 144:
    case 145:
    case 146:
    case 147:
        Section_addPage(pSection, 91);
        Section_addPage(pSection, 94);
        Section_addPage(pSection, 127);
        Section_addPage(pSection, 205);
        break;
    case 148:
        Section_addPage(pSection, 122);
        Section_addPage(pSection, 123);
        Section_addPage(pSection, 124);
        Section_addPage(pSection, 125);
        Section_addPage(pSection, 126);
        break;
    default:
        return;
    }
}

void my_Section_addActivePages(Section *pSection, u32 sectionId) {
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
        Section_addActivePage(pSection, 0);
        break;
    case 16:
    case 17:
    case 18:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 3);
        break;
    case 19:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 4);
        break;
    case 20:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 5);
        break;
    case 21:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 6);
        break;
    case 22:
        Section_addActivePage(pSection, 1);
        break;
    case 23:
    case 24:
        Section_addActivePage(pSection, 2);
        break;
    case 25:
        Section_addActivePage(pSection, 7);
        break;
    case 26:
        Section_addActivePage(pSection, 8);
        break;
    case 27:
        Section_addActivePage(pSection, 9);
        break;
    case 28:
        Section_addActivePage(pSection, 10);
        break;
    case 29:
        Section_addActivePage(pSection, 11);
        break;
    case 30:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 12);
        break;
    case 31:
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 13);
        break;
    case 32:
    case 36:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 14);
        break;
    case 33:
    case 37:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 15);
        break;
    case 34:
    case 38:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 16);
        break;
    case 35:
    case 39:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 17);
        break;
    case 40:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 18);
        break;
    case 41:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 19);
        break;
    case 42:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 20);
        break;
    case 43:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 21);
        break;
    case 44:
    case 45:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 22);
        break;
    case 46:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 54);
        break;
    case 47:
    case 50:
    case 51:
    case 52:
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 55);
        break;
    case 48:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 13);
        break;
    case 49:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 13);
        break;
    case 53:
    case 54:
    case 55:
    case 56:
        Section_addActivePage(pSection, 59);
        break;
    case 57:
    case 58:
        Section_addActivePage(pSection, 61);
        break;
    case 59:
    case 60:
        Section_addActivePage(pSection, 62);
        Section_addActivePage(pSection, 61);
        break;
    case 61:
    case 62:
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 63);
        break;
    case 63:
    case 64:
        Section_addActivePage(pSection, 84);
        Section_addActivePage(pSection, 88);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 87);
        break;
    case 65:
        Section_addActivePage(pSection, 84);
        Section_addActivePage(pSection, 88);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 90);
        break;
    case 66:
    case 67:
        Section_addActivePage(pSection, 84);
        Section_addActivePage(pSection, 88);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 101);
        break;
    case 68:
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 95);
        break;
    case 69:
    case 70:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 96);
        break;
    case 71:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 103);
        break;
    case 72:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 105);
        break;
    case 73:
    case 78:
    case 79:
    case 80:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 107);
        break;
    case 74:
    case 75:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 110);
        break;
    case 76:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 120);
        break;
    case 77:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 127);

#ifdef CHANGE_GHOST_DATA_SUPPORT
        Section_addActivePage(pSection, 112);
#endif
        break;
    case 81:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 182);
        break;
    case 82:
    case 83:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 177);
        break;
    case 84:
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 97);
        break;
    case 85:
    case 91:
    case 131:
    case 132:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 149);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 132);
        break;
    case 86:
    case 92:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 149);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 155);
        break;
    case 87:
    case 93:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 149);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 150);
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
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 92);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 144);
        break;
    case 90:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 97);
        break;
    case 104:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 64);
        break;
    case 105:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 65);
        break;
    case 106:
    case 107:
    case 110:
    case 111:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 147);
        break;
    case 108:
    case 114:
    case 115:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 18);
        break;
    case 109:
    case 118:
    case 119:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 19);
        break;
    case 112:
    case 113:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 66);
        break;
    case 116:
    case 117:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 148);
        Section_addActivePage(pSection, 58);
        Section_addActivePage(pSection, 67);
        break;
    case 120:
    case 121:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 137);
        break;
    case 122:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 162);
        break;
    case 123:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 163);
        break;
    case 124:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 164);
        break;
    case 125:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 170);
        Section_addActivePage(pSection, 171);
        Section_addActivePage(pSection, 149);
        Section_addActivePage(pSection, 168);
        break;
    case 126:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 170);
        Section_addActivePage(pSection, 171);
        Section_addActivePage(pSection, 149);
        Section_addActivePage(pSection, 176);
        break;
    case 127:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 159);
        break;
    case 128:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 159);
        break;
    case 129:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 132);
        break;
    case 130:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 167);
        Section_addActivePage(pSection, 179);
        break;
    case 133:
        Section_addActivePage(pSection, 187);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 184);
        break;
    case 134:
        Section_addActivePage(pSection, 187);
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 183);
        break;
    case 135:
        Section_addActivePage(pSection, 136);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 132);
        break;
    case 136:
    case 137:
        Section_addActivePage(pSection, 91);
        Section_addActivePage(pSection, 186);
        break;
    case 138:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 189);
        break;
    case 139:
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 188);
        break;
    case 140:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 204);
        Section_addActivePage(pSection, 192);
        break;
    case 141:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 204);
        Section_addActivePage(pSection, 202);
        break;
    case 142:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 204);
        Section_addActivePage(pSection, 197);
        break;
    case 143:
        Section_addActivePage(pSection, 93);
        Section_addActivePage(pSection, 204);
        Section_addActivePage(pSection, 198);
        break;
    case 144:
    case 145:
    case 146:
    case 147:
        Section_addActivePage(pSection, 127);
        Section_addActivePage(pSection, 91);
        Section_addActivePage(pSection, 94);
        Section_addActivePage(pSection, 205);
        break;
    default:
        return;
    }
}

PATCH_B(Section_addPages, my_Section_addPages);
PATCH_B(Section_addActivePages, my_Section_addActivePages);