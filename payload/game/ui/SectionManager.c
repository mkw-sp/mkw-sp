#include <Common.h>

void SectionManager_init(void *this);

PATCH_S16(SectionManager_init, 0x8e, 0x510 + sizeof(u32) * (1 + 11) + 0x88);
