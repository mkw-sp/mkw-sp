#include "Section.hh"

#include "game/ui/ChannelPage.hh"
#include "game/ui/LicenseSelectPage.hh"
#include "game/ui/OnlineTopPage.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ServicePackTopPage.hh"
#include "game/ui/SingleTopPage.hh"
#include "game/ui/StorageBenchmarkPage.hh"
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

        { SectionId::WifiSingle, (PageId)0x4c },
        { SectionId::WifiSingle, (PageId)0x4d },
        { SectionId::WifiSingle, (PageId)0x4e },
        { SectionId::WifiSingle, (PageId)0x4f },
        { SectionId::WifiSingle, (PageId)0x50 },
        { SectionId::WifiSingle, (PageId)0x51 },
        { SectionId::WifiSingle, (PageId)0x52 },
        { SectionId::WifiSingle, (PageId)0x5e },
        { SectionId::WifiSingle, (PageId)0x6b },
        { SectionId::WifiSingle, (PageId)0x6c },
        { SectionId::WifiSingle, (PageId)0x6d },
        { SectionId::WifiSingle, (PageId)0x76 },
        { SectionId::WifiSingle, (PageId)0x7f },
        { SectionId::WifiSingle, (PageId)0x84 },
        { SectionId::WifiSingle, (PageId)0x85 },
        { SectionId::WifiSingle, (PageId)0x86 },
        { SectionId::WifiSingle, (PageId)0x87 },
        { SectionId::WifiSingle, (PageId)0x88 },
        { SectionId::WifiSingle, (PageId)0x89 },
        { SectionId::WifiSingle, (PageId)0x8c },
        { SectionId::WifiSingle, (PageId)0x8d },
        { SectionId::WifiSingle, (PageId)0x8f },
        { SectionId::WifiSingle, (PageId)0x95 },
        { SectionId::WifiSingle, (PageId)0x96 },
        { SectionId::WifiSingle, (PageId)0x97 },
        { SectionId::WifiSingle, (PageId)0x98 },
        { SectionId::WifiSingle, (PageId)0x99 },
        { SectionId::WifiSingle, (PageId)0x9b },
        { SectionId::WifiSingle, (PageId)0x9c },
        { SectionId::WifiSingle, (PageId)0x9d },
        { SectionId::WifiSingle, (PageId)0x9e },
        { SectionId::WifiSingle, (PageId)0xa5 },
        { SectionId::WifiSingle, (PageId)0xa6 },
        { SectionId::WifiSingle, (PageId)0xa7 },


        // The channel section is repurposed into the Service Pack section. Remove some pages that
        // aren't needed anymore.
        { SectionId::ServicePack, PageId::TimeAttackTop },
        { SectionId::ServicePack, PageId::MenuMessage },
        { SectionId::ServicePack, PageId::ChannelGhost },
    };
    for (const auto &deletion : deletions) {
        if (deletion.first == m_id && deletion.second == pageId) {
            return;
        }
    }

    SP_LOG("addPage %u", pageId);

    REPLACED(addPage)(pageId);
}

void Section::addActivePage(PageId pageId) {
    std::pair<SectionId, PageId> deletions[] = {
        { SectionId::SingleChangeGhostData, PageId::CharacterSelect },

        { SectionId::WifiSingle, (PageId)0x88 },
        { SectionId::WifiSingle, PageId::GhostManager },
        { SectionId::WifiSingle, PageId::TopOverlay },
        { SectionId::WifiSingle, (PageId)0x95 },
        { SectionId::WifiSingle, (PageId)0x7f },
        { SectionId::WifiSingle, (PageId)0x84 },
    };
    for (const auto &deletion : deletions) {
        if (deletion.first == m_id && deletion.second == pageId) {
            return;
        }
    }

    SP_LOG("addActivePage %u", pageId);

    REPLACED(addActivePage)(pageId);
}

void Section::addPages(SectionId id) {
    SP_LOG("&7DEBUG: Constructing section %u (0x%x)", id, id);

    REPLACED(addPages)(id);

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

        // Mission Mode
        { SectionId::MRReplay, PageId::CompetitionPersonalLeaderboard},
        { SectionId::SingleChangeMission, PageId::MissionLevelSelect },
        { SectionId::SingleChangeMission, PageId::MissionStageSelect },
        { SectionId::SingleChangeMission, PageId::MissionPrompt },
        { SectionId::SingleChangeMission, PageId::MissionDrift },
        { SectionId::SingleChangeMission, PageId::MissionTutorial },
        { SectionId::Single, PageId::MissionLevelSelect},
        { SectionId::Single, PageId::MissionStageSelect},
        { SectionId::Single, PageId::MissionPrompt},
        { SectionId::Single, PageId::MissionDrift},
        { SectionId::Single, PageId::MissionTutorial},
        { SectionId::SingleChangeDriver, PageId::MissionLevelSelect},
        { SectionId::SingleChangeDriver, PageId::MissionStageSelect},
        { SectionId::SingleChangeDriver, PageId::MissionPrompt},
        { SectionId::SingleChangeDriver, PageId::MissionDrift},
        { SectionId::SingleChangeDriver, PageId::MissionTutorial},
        { SectionId::SingleChangeCourse, PageId::MissionLevelSelect},
        { SectionId::SingleChangeCourse, PageId::MissionStageSelect},
        { SectionId::SingleChangeCourse, PageId::MissionPrompt},
        { SectionId::SingleChangeCourse, PageId::MissionDrift},
        { SectionId::SingleChangeCourse, PageId::MissionTutorial},
        { SectionId::SingleChangeGhostData, PageId::MissionLevelSelect},
        { SectionId::SingleChangeGhostData, PageId::MissionStageSelect},
        { SectionId::SingleChangeGhostData, PageId::MissionPrompt},
        { SectionId::SingleChangeGhostData, PageId::MissionDrift},
        { SectionId::SingleChangeGhostData, PageId::MissionTutorial},

        // Change Ghost Data
        { SectionId::SingleChangeGhostData, PageId::ReadingGhostData },
        { SectionId::SingleChangeGhostData, PageId::MenuMessage },
        { SectionId::SingleChangeGhostData, (PageId)83 },
        { SectionId::SingleChangeGhostData, PageId::SingleTop },
        { SectionId::SingleChangeGhostData, (PageId)106 },
        { SectionId::SingleChangeGhostData, PageId::CupSelect },
        { SectionId::SingleChangeGhostData, PageId::CourseSelect },
        { SectionId::SingleChangeGhostData, PageId::TimeAttackTop },
        { SectionId::SingleChangeGhostData, PageId::TimeAttackGhostList },
        { SectionId::SingleChangeGhostData, PageId::VsModeSelect },
        { SectionId::SingleChangeGhostData, PageId::RaceRules },
        { SectionId::SingleChangeGhostData, PageId::TeamsOverview },
        { SectionId::SingleChangeGhostData, PageId::BattleModeSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleVehicleSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleRules },
        { SectionId::SingleChangeGhostData, PageId::BattleCupSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleCourseSelect },

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
}

void Section::addActivePages(SectionId id) {
    REPLACED(addActivePages)(id);

    std::pair<SectionId, PageId> additions[] = {
        // Change Ghost Data
        { SectionId::SingleChangeGhostData, PageId::TimeAttackTop },

        // Mission Mode
        { SectionId::SingleChangeMission, PageId::MissionLevelSelect },

        { SectionId::WifiSingle, PageId::OnlineTop },
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
    case PageId::SingleTop:
        return new SingleTopPage;
    case PageId::TimeAttackGhostList:
        return new TimeAttackGhostListPage;
    case PageId::OnlineTop:
        return new OnlineTopPage;
    case PageId::ServicePackTop:
        return new ServicePackTopPage;
    case PageId::StorageBenchmark:
        return new StorageBenchmarkPage;
    case PageId::GhostManager:
        return new GhostManagerPage;
    case PageId::Channel:
        return new ChannelPage;
    case PageId::Update:
        return new UpdatePage;
    case PageId::Settings:
        return new SettingsPage;
    default:
        return REPLACED(CreatePage)(pageId);
    }
}

} // namespace UI
