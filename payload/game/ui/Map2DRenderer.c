#include <Common.h>
#include <revolution/gx/GXEnum.h>

#include <revolution.h>

enum MinimapFormats {
    kMinimapFormats_Race = GX_TF_IA4,
    kMinimapFormats_Battle = GX_TF_RGB5A3,

    // A more advanced system could be used, perhaps a more efficient format (RGBA8 is 32bpp whereas
    // IA4 is 8bpp)
    kMinimapFormats_Override = GX_TF_RGBA8,
};

extern s32 Map2DRenderer_init__createTexBuffer;

extern void *EGG_CpuTexture_Construct(void *self, short width, short height, GXTexFmt format);

static GXTexFmt GetMinimapTextureFormat(GXTexFmt /* oldFormat */) {
    return (GXTexFmt)kMinimapFormats_Override;
}

static void *my_CpuTexture_Construct(void *self, short width, short height, GXTexFmt format) {
    return EGG_CpuTexture_Construct(self, width, height, GetMinimapTextureFormat(format));
}

PATCH_BL(Map2DRenderer_init__createTexBuffer, my_CpuTexture_Construct);