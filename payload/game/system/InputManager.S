#include <Common.S>

.global thunk_InputManager_resetMultiGhostProxies
thunk_InputManager_resetMultiGhostProxies:
    mflr r31

    mr r3, r30
    bl InputManager_resetMultiGhostProxies

    mr r3, r30 # Original instruction

    mtlr r31
    blr
.size thunk_InputManager_resetMultiGhostProxies, . - thunk_InputManager_resetMultiGhostProxies

PATCH_BL(InputManager_reset + 0x54, thunk_InputManager_resetMultiGhostProxies)

.global thunk_InputManager_calcMultiGhostPads
thunk_InputManager_calcMultiGhostPads:
    mflr r31

    mr r3, r30
    bl InputManager_calcMultiGhostPads

    li r27, 0x0 # Original instruction

    mtlr r31
    blr
.size thunk_InputManager_calcMultiGhostPads, . - thunk_InputManager_calcMultiGhostPads

PATCH_BL(InputManager_calcPads + 0xd0, thunk_InputManager_calcMultiGhostPads)

.global thunk_InputManager_calcMultiGhostProxies
thunk_InputManager_calcMultiGhostProxies:
    mflr r30

    mr r3, r29
    lbz r4, 0x4154 (r29)
    bl InputManager_calcMultiGhostProxies

    lwz r12, 0x15b4 (r29) # Original instruction

    mtlr r30
    blr
.size thunk_InputManager_calcMultiGhostProxies, . - thunk_InputManager_calcMultiGhostProxies

PATCH_BL(InputManager_calc + 0x6c, thunk_InputManager_calcMultiGhostProxies)
