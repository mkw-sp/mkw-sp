#pragma once

#include "game/system/Mii.hh"

#include <nw4r/lyt/lyt_textBox.hh>

namespace UI {

class GlyphRenderer {
private:
    REPLACE void setupColors(u32 formatId, u32 colorId);

    static GXColor GXColorS10ToGXColor(GXColorS10 color);

    nw4r::lyt::TextBox *m_textBox;
    u32 m_formatId;
    u32 m_colorId;
    u8 _0c[0x10 - 0x0c];
};
static_assert(sizeof(GlyphRenderer) == 0x10);

struct MessageInfo {
    s32 intVals[9];
    u32 messageIds[9];
    const System::Mii *miis[9];
    u8 licenseIds[9];
    u32 playerIds[9];
    const wchar_t *strings[9];
    u8 _c0[0xc4 - 0xc0];
};
static_assert(sizeof(MessageInfo) == 0xc4);

} // namespace UI
