#include "Font.h"

enum {
    FORMAT_ID_IA4 = 0,
    FORMAT_ID_RGB5A3 = 1,
    FORMAT_ID_I4 = 2,
};

enum {
    COLOR_ID_UNSPECIFIED = 0x0,
    COLOR_ID_UNUSED_RED = 0x1,
    COLOR_ID_YOR0 = 0x10,
    COLOR_ID_YOR1 = 0x11,
    COLOR_ID_YOR2 = 0x12,
    COLOR_ID_YOR3 = 0x13,
    COLOR_ID_YOR4 = 0x14,
    COLOR_ID_YOR5 = 0x15,
    COLOR_ID_YOR6 = 0x16,
    COLOR_ID_YOR7 = 0x17,
    COLOR_ID_TEAM_RED = 0x20,
    COLOR_ID_TEAM_BLUE = 0x21,
    COLOR_ID_PLAYER_1 = 0x30,
    COLOR_ID_PLAYER_2 = 0x31,
    COLOR_ID_PLAYER_3 = 0x32,
    COLOR_ID_PLAYER_4 = 0x33,
    COLOR_ID_RED = 0x40,
    COLOR_ID_GREEN = 0x50,
    COLOR_ID_BLUE = 0x51,
    COLOR_ID_PINK = 0x52,
};

static GXColor GXColorS10ToGXColor(GXColorS10 color) {
    return (GXColor) {
        .r = MAX(0, MIN(color.r, 255)),
        .g = MAX(0, MIN(color.g, 255)),
        .b = MAX(0, MIN(color.b, 255)),
        .a = MAX(0, MIN(color.a, 255)),
    };
}

void GlyphRenderer_setupColors(GlyphRenderer *self, u32 formatId, u32 colorId);

static void my_GlyphRenderer_setupColors(GlyphRenderer *self, u32 formatId, u32 colorId) {
    if (formatId != self->formatId) {
        switch (formatId) {
        case FORMAT_ID_IA4:
            GXSetNumTevStages(2);

            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C2, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A1, GX_CA_A2, GX_CA_TEXA, GX_CA_ZERO);

            GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);

            break;
        case FORMAT_ID_RGB5A3:
            GXSetNumTevStages(1);

            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

            break;
        case FORMAT_ID_I4:
            GXSetNumTevStages(3);

            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_SUBHALF, GX_CS_SCALE_2, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);

            GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_8_8);

            GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C2, GX_CC_CPREV, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_A1, GX_CA_A2, GX_CA_APREV, GX_CA_ZERO);

            GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);

            break;
        }
        self->formatId = formatId;
    }

    if (formatId == FORMAT_ID_RGB5A3) {
        return;
    }

    if (colorId == self->colorId) {
        return;
    }

    lyt_Material *material = self->textBox->vtable->GetMaterial((lyt_Pane *)self->textBox);
    GXColor bgColor = GXColorS10ToGXColor(material->tevColors[0]);
    GXSetTevColor(GX_TEVREG1, bgColor);
    GXColor formatColor;
    switch (colorId) {
    case COLOR_ID_UNUSED_RED:
        formatColor = (GXColor) { .r = 255, .g = 0, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR0:
        formatColor = (GXColor) { .r = 255, .g = 255, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR1:
        formatColor = (GXColor) { .r = 255, .g = 218, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR2:
        formatColor = (GXColor) { .r = 255, .g = 182, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR3:
        formatColor = (GXColor) { .r = 255, .g = 145, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR4:
        formatColor = (GXColor) { .r = 255, .g = 109, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR5:
        formatColor = (GXColor) { .r = 255, .g = 73, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR6:
        formatColor = (GXColor) { .r = 255, .g = 36, .b = 0, .a = 255 };
        break;
    case COLOR_ID_YOR7:
        formatColor = (GXColor) { .r = 255, .g = 0, .b = 0, .a = 255 };
        break;
    case COLOR_ID_TEAM_RED:
        formatColor = (GXColor) { .r = 255, .g = 70, .b = 70, .a = 255 };
        break;
    case COLOR_ID_TEAM_BLUE:
        formatColor = (GXColor) { .r = 100, .g = 180, .b = 255, .a = 255 };
        break;
    case COLOR_ID_PLAYER_1:
        formatColor = (GXColor) { .r = 255, .g = 255, .b = 0, .a = 255 };
        break;
    case COLOR_ID_PLAYER_2:
        formatColor = (GXColor) { .r = 0, .g = 111, .b = 255, .a = 255 };
        break;
    case COLOR_ID_PLAYER_3:
        formatColor = (GXColor) { .r = 255, .g = 0, .b = 0, .a = 255 };
        break;
    case COLOR_ID_PLAYER_4:
        formatColor = (GXColor) { .r = 0, .g = 186, .b = 0, .a = 255 };
        break;
    case COLOR_ID_RED:
        formatColor = (GXColor) { .r = 234, .g = 117, .b = 125, .a = 255 };
        break;
    case COLOR_ID_GREEN:
        formatColor = (GXColor) { .r =   0, .g = 255, .b =   0, .a = 255 };
        break;
    case COLOR_ID_BLUE:
        formatColor = (GXColor) { .r =   0, .g =  85, .b = 255, .a = 255 };
        break;
    case COLOR_ID_PINK:
        formatColor = (GXColor) { .r = 255, .g =   0, .b = 255, .a = 255 };
        break;
    default:
        formatColor = (GXColor) { .r = 255, .g = 255, .b = 255, .a = 255 };
        break;
    }
    GXColor fgColor = GXColorS10ToGXColor(material->tevColors[1]);
    fgColor.r = (fgColor.r * formatColor.r) / 256;
    fgColor.g = (fgColor.g * formatColor.g) / 256;
    fgColor.b = (fgColor.b * formatColor.b) / 256;
    GXSetTevColor(GX_TEVREG2, fgColor);
    self->colorId = colorId;
}
PATCH_B(GlyphRenderer_setupColors, my_GlyphRenderer_setupColors);
