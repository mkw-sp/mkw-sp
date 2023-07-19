extern "C" {
#include "tpl.h"
}

extern "C" void TPLBind(TPLPalette *tpl) {
    constexpr u32 IMAGE_DESCRIPTOR_ARRAY_OFFSET = 0x0000000C;
    assert(reinterpret_cast<u32>(tpl->imageDescriptorArray) == IMAGE_DESCRIPTOR_ARRAY_OFFSET);

    REPLACED(TPLBind)(tpl);
}
