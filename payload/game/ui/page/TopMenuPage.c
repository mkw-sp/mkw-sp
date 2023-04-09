#include <Common.h>

void *TopMenuPage_ct(void *this);

// Hide the channel button
PATCH_S16(TopMenuPage_ct, 0x7e, 3);
