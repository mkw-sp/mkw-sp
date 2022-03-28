#include "JugemManager.h"

#include "../ui/SectionManager.h"

#include "../ui/page/RacePage.h"

static void my_JugemManager_calc(JugemManager *this) {
    for (u32 i = 0; i < this->count; i++) {
        this->jugems[i]->vt->calc(this->jugems[i]);
    }
}
PATCH_B(JugemManager_calc, my_JugemManager_calc);

static void my_JugemManager_setVisible(JugemManager *this, u32 playerId, bool UNUSED(visible)) {
    for (u32 i = 0; i < this->count; i++) {
        if (Jugem_getPlayerId(this->jugems[i]) == playerId) {
            Jugem_setVisible(this->jugems[i], false);
        }
    }
}
PATCH_B(JugemManager_setVisible, my_JugemManager_setVisible);
