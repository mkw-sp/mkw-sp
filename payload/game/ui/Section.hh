#pragma once

#include "game/ui/Page.hh"
#include "game/ui/SectionId.hh"

namespace UI {

class Section {
private:
    SectionId m_id;
    u8 _004[0x008 - 0x004];
    Page *m_pages[PageId::Max];
    u8 _354[0x408 - 0x354];
};
static_assert(sizeof(Section) == 0x408);

} // namespace UI
