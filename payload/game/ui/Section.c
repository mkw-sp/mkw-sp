#include "Section.h"

#include "TimeAttackGhostListPage.h"

PATCH_S16(Section_createPage, 0xa4e, sizeof(TimeAttackGhostListPage));
