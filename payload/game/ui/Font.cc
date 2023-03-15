#include "Font.hh"

#include "game/system/SaveManager.hh"

#include <algorithm>

namespace UI {

namespace FormatId {

enum {
    IA4 = 0,
    RGB5A3 = 1,
    I4 = 2,
};

} // namespace FormatId

namespace ColorId {

enum {
    Unspecified = 0x0,
    UnusedRed = 0x1,
    YOR0 = 0x10,
    YOR1 = 0x11,
    YOR2 = 0x12,
    YOR3 = 0x13,
    YOR4 = 0x14,
    YOR5 = 0x15,
    YOR6 = 0x16,
    YOR7 = 0x17,
    TeamRed = 0x20,
    TeamBlue = 0x21,
    Player1 = 0x30,
    Player2 = 0x31,
    Player3 = 0x32,
    Player4 = 0x33,
    Red = 0x40,
    Green = 0x50,
    Blue = 0x51,
    Pink = 0x52,
};

} // namespace ColorId

void GlyphRenderer::setupColors(u32 formatId, u32 colorId) {
    if (formatId != m_formatId) {
        switch (formatId) {
        case FormatId::IA4:
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
        case FormatId::RGB5A3:
            GXSetNumTevStages(1);

            GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                    GX_TEVPREV);
            GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
            GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

            break;
        case FormatId::I4:
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
        m_formatId = formatId;
    }

    if (formatId == FormatId::RGB5A3) {
        return;
    }

    if (colorId == m_colorId) {
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ColorPalette>();
    bool colorblind = setting == SP::ClientSettings::ColorPalette::Colorblind;

    auto *material = m_textBox->getMaterial();
    GXColor bgColor = GXColorS10ToGXColor(material->tevColors[0]);
    GXSetTevColor(GX_TEVREG1, bgColor);
    GXColor formatColor;
    switch (colorId) {
    case ColorId::UnusedRed:
        formatColor = (GXColor){.r = 255, .g = 0, .b = 0, .a = 255};
        break;
    case ColorId::YOR0:
        formatColor = (GXColor){.r = 255, .g = 255, .b = 0, .a = 255};
        break;
    case ColorId::YOR1:
        formatColor = (GXColor){.r = 255, .g = 218, .b = 0, .a = 255};
        break;
    case ColorId::YOR2:
        formatColor = (GXColor){.r = 255, .g = 182, .b = 0, .a = 255};
        break;
    case ColorId::YOR3:
        formatColor = (GXColor){.r = 255, .g = 145, .b = 0, .a = 255};
        break;
    case ColorId::YOR4:
        formatColor = (GXColor){.r = 255, .g = 109, .b = 0, .a = 255};
        break;
    case ColorId::YOR5:
        formatColor = (GXColor){.r = 255, .g = 73, .b = 0, .a = 255};
        break;
    case ColorId::YOR6:
        formatColor = (GXColor){.r = 255, .g = 36, .b = 0, .a = 255};
        break;
    case ColorId::YOR7:
        formatColor = (GXColor){.r = 255, .g = 0, .b = 0, .a = 255};
        break;
    case ColorId::TeamRed:
        formatColor = (GXColor){.r = 255, .g = 70, .b = 70, .a = 255};
        break;
    case ColorId::TeamBlue:
        formatColor = (GXColor){.r = 100, .g = 180, .b = 255, .a = 255};
        break;
    case ColorId::Player1:
        if (colorblind) {
            formatColor = (GXColor){.r = 240, .g = 228, .b = 66, .a = 255};
        } else {
            formatColor = (GXColor){.r = 255, .g = 255, .b = 0, .a = 255};
        }
        break;
    case ColorId::Player2:
        if (colorblind) {
            formatColor = (GXColor){.r = 0, .g = 114, .b = 178, .a = 255};
        } else {
            formatColor = (GXColor){.r = 0, .g = 111, .b = 255, .a = 255};
        }
        break;
    case ColorId::Player3:
        if (colorblind) {
            formatColor = (GXColor){.r = 213, .g = 94, .b = 0, .a = 255};
        } else {
            formatColor = (GXColor){.r = 255, .g = 0, .b = 0, .a = 255};
        }
        break;
    case ColorId::Player4:
        if (colorblind) {
            formatColor = (GXColor){.r = 0, .g = 158, .b = 115, .a = 255};
        } else {
            formatColor = (GXColor){.r = 0, .g = 186, .b = 0, .a = 255};
        }
        break;
    case ColorId::Red:
        formatColor = (GXColor){.r = 234, .g = 117, .b = 125, .a = 255};
        break;
    case ColorId::Green:
        if (colorblind) {
            formatColor = (GXColor){.r = 0, .g = 158, .b = 115, .a = 255};
        } else {
            formatColor = (GXColor){.r = 0, .g = 255, .b = 0, .a = 255};
        }
        break;
    case ColorId::Blue:
        if (colorblind) {
            formatColor = (GXColor){.r = 0, .g = 114, .b = 178, .a = 255};
        } else {
            formatColor = (GXColor){.r = 0, .g = 170, .b = 255, .a = 255};
        }
        break;
    case ColorId::Pink:
        if (colorblind) {
            formatColor = (GXColor){.r = 204, .g = 121, .b = 167, .a = 255};
        } else {
            formatColor = (GXColor){.r = 255, .g = 0, .b = 255, .a = 255};
        }
        break;
    default:
        formatColor = (GXColor){.r = 255, .g = 255, .b = 255, .a = 255};
        break;
    }
    GXColor fgColor = GXColorS10ToGXColor(material->tevColors[1]);
    fgColor.r = (fgColor.r * formatColor.r) / 256;
    fgColor.g = (fgColor.g * formatColor.g) / 256;
    fgColor.b = (fgColor.b * formatColor.b) / 256;
    GXSetTevColor(GX_TEVREG2, fgColor);
    m_colorId = colorId;
}

GXColor GlyphRenderer::GXColorS10ToGXColor(GXColorS10 color) {
    return (GXColor){
            .r = static_cast<u8>(std::clamp<s16>(color.r, 0, 255)),
            .g = static_cast<u8>(std::clamp<s16>(color.g, 0, 255)),
            .b = static_cast<u8>(std::clamp<s16>(color.b, 0, 255)),
            .a = static_cast<u8>(std::clamp<s16>(color.a, 0, 255)),
    };
}

} // namespace UI
