#include "Jugem.h"

typedef struct {
    u8 _00[0x14 - 0x00];
    Jugem *jugems[12]; // Extended
    u32 count; // Moved
} JugemManager;

void JugemManager_calc(JugemManager *this);

void JugemManager_setVisible(JugemManager *this, u32 playerId, bool visible);
