#include "Section.h"

#include "LicenseSelectPage.h"
#include "LicenseSettingsPage.h"
#include "TimeAttackGhostListPage.h"
#include "TimeAttackRulesPage.h"

PATCH_S16(Section_createPage, 0x92e, sizeof(LicenseSelectPage));
PATCH_S16(Section_createPage, 0x9a6, sizeof(TimeAttackRulesPage));
PATCH_S16(Section_createPage, 0xa4e, sizeof(TimeAttackGhostListPage));
PATCH_S16(Section_createPage, 0x12d6, sizeof(LicenseSettingsPage));
