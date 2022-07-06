#include "JugemManager.h"

#include "../ui/SectionManager.h"

#include "../ui/page/RacePage.h"

static void my_JugemManager_calc(JugemManager *this) {
    for (u32 i = 0; i < this->count; i++) {
        this->jugems[i]->visible = true;
        if (s_sectionManager->currentSection->id == SECTION_ID_GHOST_REPLAY) {
            if (Jugem_getPlayerId(this->jugems[i]) != s_racePage->watchedPlayerId) {
                this->jugems[i]->visible = false;
            }
        }

        this->jugems[i]->vt->calc(this->jugems[i]);
    }
}
PATCH_B(JugemManager_calc, my_JugemManager_calc);

static void my_JugemManager_setVisible(JugemManager *this, u32 playerId, bool visible) {
    for (u32 i = 0; i < this->count; i++) {
        if (Jugem_getPlayerId(this->jugems[i]) == playerId) {
            if (!this->jugems[i]->visible) {
                visible = false;
            }
            Jugem_setVisible(this->jugems[i], visible);
        }
    }
}
PATCH_B(JugemManager_setVisible, my_JugemManager_setVisible);
