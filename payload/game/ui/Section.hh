#pragma once

#include "game/ui/AwaitPage.hh"
#include "game/ui/ConfirmPage.hh"
#include "game/ui/GhostManagerPage.hh"
#include "game/ui/MessagePage.hh"
#include "game/ui/ModelPage.hh"
#include "game/ui/Option.hh"
#include "game/ui/Page.hh"
#include "game/ui/SectionId.hh"
#include "game/ui/SettingsPage.hh"

namespace UI {

class Section {
private:
    template <PageId P>
    struct PageIdHelper;

public:
    Page *page(PageId pageId);

    template <PageId P>
    PageIdHelper<P>::type *page() {
        return reinterpret_cast<PageIdHelper<P>::type *>(m_pages[static_cast<size_t>(P)]);
    }

    SectionId id() const { return m_id; }

    static u32 GetSceneId(SectionId id);

private:
    void REPLACED(addPage)(PageId pageId);
    REPLACE void addPage(PageId pageId);
    void REPLACED(addActivePage)(PageId pageId);
    REPLACE void addActivePage(PageId pageId);
    void REPLACED(addPages)(SectionId id);
    REPLACE void addPages(SectionId id);
    void REPLACED(addActivePages)(SectionId id);
    REPLACE void addActivePages(SectionId id);

    static Page *REPLACED(CreatePage)(PageId pageId);
    static REPLACE Page *CreatePage(PageId pageId);

    SectionId m_id;
    u8 _004[0x008 - 0x004];
    Page *m_pages[static_cast<size_t>(PageId::Max)];
    u8 _354[0x408 - 0x354];
};
static_assert(sizeof(Section) == 0x408);

template <>
struct Section::PageIdHelper<PageId::Confirm> {
    using type = ConfirmPage;
};

template <>
struct Section::PageIdHelper<PageId::Model> {
    using type = ModelPage;
};

template <>
struct Section::PageIdHelper<PageId::GhostManager> {
    using type = GhostManagerPage;
};

template <>
struct Section::PageIdHelper<PageId::OptionExplanation> {
    using type = OptionExplanationPage;
};

template <>
struct Section::PageIdHelper<PageId::OptionSelect2> {
    using type = OptionSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::OptionSelect3> {
    using type = OptionSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::OptionAwait> {
    using type = MenuAwaitPage;
};

template <>
struct Section::PageIdHelper<PageId::OptionMessage> {
    using type = MenuMessagePage;
};

template <>
struct Section::PageIdHelper<PageId::OptionConfirm> {
    using type = ConfirmPage;
};

template <>
struct Section::PageIdHelper<PageId::Settings> {
    using type = SettingsPage;
};

} // namespace UI
