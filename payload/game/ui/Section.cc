#include "Section.hh"

#include "game/ui/ChannelPage.hh"
#include "game/ui/LicenseSelectPage.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ServicePackTopPage.hh"
#include "game/ui/TimeAttackGhostListPage.hh"
#include "game/ui/UpdatePage.hh"

namespace UI {

Page *Section::page(PageId pageId) {
    return m_pages[static_cast<size_t>(pageId)];
}

void Section::addPage(PageId pageId) {
    std::pair<SectionId, PageId> deletions[] = {
        // The game has 5 pages for the records, we only need 1 for the settings. Remove the 4
        // others.
        { SectionId::LicenseSettings, PageId::LicenseRecordsFavorites },
        { SectionId::LicenseSettings, PageId::LicenseRecordsFriends },
        { SectionId::LicenseSettings, PageId::LicenseRecordsWFC },
        { SectionId::LicenseSettings, PageId::LicenseRecordsOther },
        // The channel section is repurposed into the Service Pack section. Remove some pages that
        // aren't needed anymore.
        { SectionId::ServicePack, PageId::TimeAttackTop },
        { SectionId::ServicePack, PageId::MenuMessage },
        { SectionId::ServicePack, PageId::ChannelRanking },
        { SectionId::ServicePack, PageId::ChannelGhost },
    };
    for (const auto &deletion : deletions) {
        if (deletion.first == m_id && deletion.second == pageId) {
            return;
        }
    }

    REPLACED(addPage)(pageId);
}

void Section::addPages(SectionId id) {
    SP_LOG("&7DEBUG: Constructing section %u (0x%x)", id, id);

    std::pair<SectionId, PageId> additions[] = {
        // Always show the quit confirmation page
        { SectionId::TA, PageId::ConfirmQuit },
        { SectionId::VS1P, PageId::ConfirmQuit },
        { SectionId::TeamVS1P, PageId::ConfirmQuit },
        { SectionId::Battle1P, PageId::ConfirmQuit },
        { SectionId::TAReplay, PageId::ConfirmQuit },
        // Support changing settings in-race
        { SectionId::GP, PageId::Settings },
        { SectionId::TA, PageId::Settings },
        { SectionId::VS1P, PageId::Settings },
        { SectionId::VS2P, PageId::Settings },
        { SectionId::VS3P, PageId::Settings },
        { SectionId::VS4P, PageId::Settings },
        { SectionId::TeamVS1P, PageId::Settings },
        { SectionId::TeamVS2P, PageId::Settings },
        { SectionId::TeamVS3P, PageId::Settings },
        { SectionId::TeamVS4P, PageId::Settings },
        { SectionId::Battle1P, PageId::Settings },
        { SectionId::Battle2P, PageId::Settings },
        { SectionId::Battle3P, PageId::Settings },
        { SectionId::Battle4P, PageId::Settings },
        { SectionId::MRReplay, PageId::Settings },
        { SectionId::TournamentReplay, PageId::Settings },
        { SectionId::GPReplay, PageId::Settings },
        { SectionId::TAReplay, PageId::Settings },
        { SectionId::GhostTA, PageId::Settings },
        { SectionId::GhostTAOnline, PageId::Settings },
        { SectionId::GhostReplayChannel, PageId::Settings },
        { SectionId::GhostReplayDownload, PageId::Settings },
        { SectionId::GhostReplay, PageId::Settings },
        // Support changing settings from the ghost list
        { SectionId::Single, PageId::Settings },
        { SectionId::SingleChangeDriver, PageId::Settings },
        { SectionId::SingleChangeCourse, PageId::Settings },
        { SectionId::SingleChangeGhostData, PageId::Settings },
        // The channel section is repurposed into the Service Pack section. Add some additional
        // pages we need.
        { SectionId::ServicePack, PageId::OptionExplanation },
        { SectionId::ServicePack, PageId::OptionSelect2 },
        { SectionId::ServicePack, PageId::OptionAwait },
        { SectionId::ServicePack, PageId::OptionMessage },
        { SectionId::ServicePack, PageId::OptionConfirm },
        { SectionId::ServicePack, PageId::Update },
        { SectionId::ServicePack, PageId::Channel },
    };
    for (const auto &addition : additions) {
        if (addition.first == id) {
            addPage(addition.second);
        }
    }

    REPLACED(addPages)(id);
}

void Section::addActivePages(SectionId id) {
    REPLACED(addActivePages)(id);

    std::pair<SectionId, PageId> additions[] = {
        // Complete the "Change Ghost Data" section (repurposed "Change Mission")
        { SectionId::SingleChangeGhostData, PageId::TimeAttackTop },
    };
    for (const auto &addition : additions) {
        if (addition.first == id) {
            addActivePage(addition.second);
        }
    }
}

Page *Section::CreatePage(PageId pageId) {
    switch (pageId) {
    case PageId::LicenseSelect:
        return new LicenseSelectPage;
    case PageId::Settings:
        return new SettingsPage;
    case PageId::TimeAttackGhostList:
        return new TimeAttackGhostListPage;
    case PageId::ServicePackTop:
        return new ServicePackTopPage;
    case PageId::GhostManager:
        return new GhostManagerPage;
    case PageId::Channel:
        return new ChannelPage;
    case PageId::Update:
        return new UpdatePage;
    default:
        return REPLACED(CreatePage)(pageId);
    }
}

} // namespace UI
