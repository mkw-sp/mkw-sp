#include "TabControl.h"

void TabOptionButton_select(TabOptionButton *this, u32 localPlayerId) {
    MultiControlInputManager_select(this->inputManager.parent, localPlayerId, &this->inputManager,
            -1);
}
