#include "g3d_resmat.h"
#include "MSan.h"

#include <Common.h>

uint8_t *HookMatInit(uint8_t **pMat) {
    sanitizeMaterial(*pMat);

    // Original instruction: lwz r3, 0(r3)
    return *pMat;
}

PATCH_B(ResMat_Init, HookMatInit);
