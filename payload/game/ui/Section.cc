#include "Section.hh"

#include "game/ui/ConfirmPage.hh"

namespace UI {

template <>
ConfirmPage *Section::page() {
    return reinterpret_cast<ConfirmPage *>(m_pages[static_cast<size_t>(PageId::Confirm)]);
}

} // namespace UI
