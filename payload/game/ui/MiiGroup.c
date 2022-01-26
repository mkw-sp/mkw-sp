#include "MiiGroup.h"

void MiiGroup_swap(MiiGroup *this, u32 i0, u32 i1) {
    Mii *tmp = this->miis[i0];
    this->miis[i0] = this->miis[i1];
    this->miis[i1] = tmp;
    for (u32 i = 0; i < 7; i++) {
        if (this->textures[i] != NULL) {
            MiiTexture tmp = this->textures[i][i0];
            this->textures[i][i0] = this->textures[i][i1];
            this->textures[i][i1] = tmp;
        }
    }
}
