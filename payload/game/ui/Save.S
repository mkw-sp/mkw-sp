#include <Common.S>

// Update message id
PATCH_REPLACE_START(SaveFailedPage_onActivate, 0xf4)
    li r4, 10075
PATCH_REPLACE_END(SaveFailedPage_onActivate, 0xf4)
