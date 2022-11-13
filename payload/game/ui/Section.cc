#include "Section.hh"

#include "game/ui/AwardPage.hh"
#include "game/ui/ChannelPage.hh"
#include "game/ui/CourseSelectPage.hh"
#include "game/ui/DirectConnectionPage.hh"
#include "game/ui/FriendMatchingPage.hh"
#include "game/ui/FriendRoomBackPage.hh"
#include "game/ui/FriendRoomMessageSelectPage.hh"
#include "game/ui/FriendRoomPage.hh"
#include "game/ui/FriendRoomRulesPage.hh"
#include "game/ui/LicenseSelectPage.hh"
#include "game/ui/ModelRenderPage.hh"
#include "game/ui/MultiTeamSelectPage.hh"
#include "game/ui/MultiTopPage.hh"
#include "game/ui/OnlineTeamSelectPage.hh"
#include "game/ui/OnlineTopPage.hh"
#include "game/ui/RoulettePage.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/ServicePackToolsPage.hh"
#include "game/ui/ServicePackTopPage.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/SingleTopPage.hh"
#include "game/ui/StorageBenchmarkPage.hh"
#include "game/ui/TeamConfirmPage.hh"
#include "game/ui/TimeAttackGhostListPage.hh"
#include "game/ui/UpdatePage.hh"
#include "game/ui/VotingBackPage.hh"
#include "game/ui/page/DriftSelectPage.hh"
#include "game/ui/page/ResultTeamVSTotalPage.hh"

namespace UI {

Page *Section::page(PageId pageId) {
    return m_pages[static_cast<size_t>(pageId)];
}

bool Section::isPageActive(PageId pageId) const {
    for (u32 i = 0; i < m_activePageCount; i++) {
        if (m_activePages[i]->id() == pageId) {
            return true;
        }
    }
    return false;
}

u32 Section::GetSceneId(SectionId id) {
    switch (id) {
    case SectionId::Thumbnails:
        return 2; // Race
    default:
        return REPLACED(GetSceneId)(id);
    }
}

const char *Section::GetResourceName(SectionId id) {
    switch (id) {
    case SectionId::Thumbnails:
        return "/Scene/UI/Race";
    default:
        return REPLACED(GetResourceName)(id);
    }
}

bool Section::HasRoomClient(SectionId sectionId) {
    switch(sectionId) {
    case SectionId::OnlineSingle:
    case SectionId::OnlineMulti:
    case SectionId::Voting1PVS:
    case SectionId::Voting1PBalloon:
    case SectionId::Voting1PCoin:
    case SectionId::Voting2PVS:
    case SectionId::Voting2PBalloon:
    case SectionId::Voting2PCoin:
        return true;
    default:
        return false;
    }
}

bool Section::HasRoomServer(SectionId sectionId) {
    switch (sectionId) {
    case SectionId::OnlineServer:
    case SectionId::VotingServer:
        return true;
    default:
        return false;
    }
}

void Section::addPage(PageId pageId) {
    std::pair<SectionId, PageId> deletions[] = {
        // The game has 5 pages for the records, we only need 1 for the settings. Remove the 4
        // others.
        { SectionId::LicenseSettings, PageId::SettingsPopup },
        { SectionId::LicenseSettings, PageId::LicenseRecordsFriends },
        { SectionId::LicenseSettings, PageId::LicenseRecordsWFC },
        { SectionId::LicenseSettings, PageId::LicenseRecordsOther },

        { SectionId::Single, PageId::VSSelect },
        { SectionId::Single, PageId::VSSetting },
        { SectionId::Single, PageId::BattleSetting },
        { SectionId::Single, (PageId)0x6f },
        { SectionId::SingleChangeDriver, PageId::VSSelect },
        { SectionId::SingleChangeDriver, PageId::VSSetting },
        { SectionId::SingleChangeDriver, PageId::BattleSetting },
        { SectionId::SingleChangeCourse, PageId::VSSelect },
        { SectionId::SingleChangeCourse, PageId::VSSetting },
        { SectionId::SingleChangeCourse, PageId::BattleSetting },
        { SectionId::SingleChangeMission, PageId::VSSelect },
        { SectionId::SingleChangeMission, PageId::VSSetting },
        { SectionId::SingleChangeMission, PageId::BattleSetting },
        { SectionId::Multi, PageId::VSSelect },
        { SectionId::Multi, PageId::VSSetting },
        { SectionId::Multi, PageId::BattleSetting },

        { SectionId::OnlineSingle, (PageId)0x4c },
        { SectionId::OnlineSingle, (PageId)0x4f },
        { SectionId::OnlineSingle, (PageId)0x50 },
        { SectionId::OnlineSingle, (PageId)0x52 },
        { SectionId::OnlineSingle, (PageId)0x76 },
        { SectionId::OnlineSingle, (PageId)0x85 },
        { SectionId::OnlineSingle, (PageId)0x86 },
        { SectionId::OnlineSingle, (PageId)0x87 },
        { SectionId::OnlineSingle, (PageId)0x89 },
        { SectionId::OnlineSingle, (PageId)0x8c },
        { SectionId::OnlineSingle, (PageId)0x8d },
        { SectionId::OnlineSingle, (PageId)0x8f },
        { SectionId::OnlineSingle, (PageId)0x96 },
        { SectionId::OnlineSingle, (PageId)0x97 },
        { SectionId::OnlineSingle, (PageId)0x98 },
        { SectionId::OnlineSingle, (PageId)0x99 },
        { SectionId::OnlineSingle, (PageId)0xa5 },
        { SectionId::OnlineSingle, (PageId)0xa6 },
        { SectionId::OnlineSingle, (PageId)0xa7 },

        { SectionId::OnlineMulti, (PageId)0x4c },
        { SectionId::OnlineMulti, (PageId)0x4f },
        { SectionId::OnlineMulti, (PageId)0x50 },
        { SectionId::OnlineMulti, (PageId)0x52 },
        { SectionId::OnlineMulti, (PageId)0x6b },
        { SectionId::OnlineMulti, (PageId)0x76 },
        { SectionId::OnlineMulti, (PageId)0x7f },
        { SectionId::OnlineMulti, (PageId)0x81 },
        { SectionId::OnlineMulti, (PageId)0x82 },
        { SectionId::OnlineMulti, (PageId)0x85 },
        { SectionId::OnlineMulti, (PageId)0x86 },
        { SectionId::OnlineMulti, (PageId)0x87 },
        { SectionId::OnlineMulti, (PageId)0x89 },
        { SectionId::OnlineMulti, (PageId)0x8c },
        { SectionId::OnlineMulti, (PageId)0x8d },
        { SectionId::OnlineMulti, (PageId)0x8f },
        { SectionId::OnlineMulti, (PageId)0x96 },
        { SectionId::OnlineMulti, (PageId)0x97 },
        { SectionId::OnlineMulti, (PageId)0x98 },
        { SectionId::OnlineMulti, (PageId)0x99 },
        { SectionId::OnlineMulti, (PageId)0xa5 },
        { SectionId::OnlineMulti, (PageId)0xa6 },
        { SectionId::OnlineMulti, (PageId)0xa7 },

        { SectionId::Voting1PVS, (PageId)0x50 },
        { SectionId::Voting1PVS, (PageId)0x51 },
        { SectionId::Voting1PVS, (PageId)0x88 },
        { SectionId::Voting1PVS, (PageId)0x91 },

        { SectionId::VotingServer, (PageId)0x4e },
        { SectionId::VotingServer, (PageId)0x51 },
        { SectionId::VotingServer, (PageId)0x6e },
        { SectionId::VotingServer, (PageId)0x6f },
        { SectionId::VotingServer, (PageId)0x88 },
        { SectionId::VotingServer, (PageId)0x91 },

        { SectionId::Online1PVS, (PageId)0x44 },
        { SectionId::Online1PVS, (PageId)0x48 },
        { SectionId::Online1PVS, (PageId)0x88 },

        // The channel section is repurposed into the Service Pack section. Remove some pages that
        // aren't needed anymore.
        { SectionId::ServicePack, PageId::TimeAttackTop },

        { SectionId::OnlineServer, (PageId)0x4c },
        { SectionId::OnlineServer, (PageId)0x4f },
        { SectionId::OnlineServer, (PageId)0x50 },
        { SectionId::OnlineServer, (PageId)0x51 },
        { SectionId::OnlineServer, (PageId)0x52 },
        { SectionId::OnlineServer, (PageId)0x6b },
        { SectionId::OnlineServer, (PageId)0x6c },
        { SectionId::OnlineServer, (PageId)0x6d },
        { SectionId::OnlineServer, (PageId)0x76 },
        { SectionId::OnlineServer, (PageId)0x7f },
        { SectionId::OnlineServer, (PageId)0x84 },
        { SectionId::OnlineServer, (PageId)0x85 },
        { SectionId::OnlineServer, (PageId)0x86 },
        { SectionId::OnlineServer, (PageId)0x87 },
        { SectionId::OnlineServer, (PageId)0x89 },
        { SectionId::OnlineServer, (PageId)0x8e },
        { SectionId::OnlineServer, (PageId)0x8f },
        { SectionId::OnlineServer, (PageId)0x96 },
        { SectionId::OnlineServer, (PageId)0x97 },
        { SectionId::OnlineServer, (PageId)0x98 },
        { SectionId::OnlineServer, (PageId)0x99 },
        { SectionId::OnlineServer, (PageId)0x9e },
        { SectionId::OnlineServer, (PageId)0xa5 },
        { SectionId::OnlineServer, (PageId)0xa6 },
        { SectionId::OnlineServer, (PageId)0xa7 },
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

        { SectionId::OnlineSingle, (PageId)0x88 },
        { SectionId::OnlineSingle, PageId::GhostManager },
        { SectionId::OnlineSingle, (PageId)0x7f },
        { SectionId::OnlineSingle, (PageId)0x84 },

        { SectionId::OnlineMulti, (PageId)0x88 },
        { SectionId::OnlineMulti, PageId::GhostManager },
        { SectionId::OnlineMulti, (PageId)0x7f },
        { SectionId::OnlineMulti, (PageId)0x84 },

        { SectionId::OnlineServer, (PageId)0x88 },
        { SectionId::OnlineServer, PageId::GhostManager },
        { SectionId::OnlineServer, (PageId)0x7f },
        { SectionId::OnlineServer, (PageId)0x84 },

        { SectionId::Voting1PVS, (PageId)0x88 },

        { SectionId::VotingServer, (PageId)0x88 },

        { SectionId::Online1PVS, (PageId)0x88 },
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

        { SectionId::VS1P, PageId::ResultTeamVSTotal },
        { SectionId::VS2P, PageId::ResultTeamVSTotal },
        { SectionId::VS3P, PageId::ResultTeamVSTotal },
        { SectionId::VS4P, PageId::ResultTeamVSTotal },

        // Support changing settings in-race
        { SectionId::GP, PageId::MenuSettings },
        { SectionId::TA, PageId::MenuSettings },
        { SectionId::VS1P, PageId::MenuSettings },
        { SectionId::VS2P, PageId::MenuSettings },
        { SectionId::VS3P, PageId::MenuSettings },
        { SectionId::VS4P, PageId::MenuSettings },
        { SectionId::TeamVS1P, PageId::MenuSettings },
        { SectionId::TeamVS2P, PageId::MenuSettings },
        { SectionId::TeamVS3P, PageId::MenuSettings },
        { SectionId::TeamVS4P, PageId::MenuSettings },
        { SectionId::Battle1P, PageId::MenuSettings },
        { SectionId::Battle2P, PageId::MenuSettings },
        { SectionId::Battle3P, PageId::MenuSettings },
        { SectionId::Battle4P, PageId::MenuSettings },
        { SectionId::MRReplay, PageId::MenuSettings },
        { SectionId::TournamentReplay, PageId::MenuSettings },
        { SectionId::GPReplay, PageId::MenuSettings },
        { SectionId::TAReplay, PageId::MenuSettings },
        { SectionId::GhostTA, PageId::MenuSettings },
        { SectionId::GhostTAOnline, PageId::MenuSettings },
        { SectionId::GhostReplayChannel, PageId::MenuSettings },
        { SectionId::GhostReplayDownload, PageId::MenuSettings },
        { SectionId::GhostReplay, PageId::MenuSettings },

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
        { SectionId::SingleChangeGhostData, PageId::CourseSelect },
        { SectionId::SingleChangeGhostData, PageId::TimeAttackTop },
        { SectionId::SingleChangeGhostData, PageId::TimeAttackGhostList },
        { SectionId::SingleChangeGhostData, PageId::TeamConfirm },
        { SectionId::SingleChangeGhostData, PageId::BattleModeSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleVehicleSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleCupSelect },
        { SectionId::SingleChangeGhostData, PageId::BattleCourseSelect },

        { SectionId::Single, PageId::MenuSettings },
        { SectionId::SingleChangeDriver, PageId::MenuSettings },
        { SectionId::SingleChangeCourse, PageId::MenuSettings },
        { SectionId::SingleChangeGhostData, PageId::MenuSettings },
        { SectionId::Multi, PageId::MenuSettings },

        { SectionId::OnlineSingle, PageId::FriendRoomRules },
        { SectionId::OnlineSingle, PageId::MenuSettings },
        { SectionId::OnlineSingle, PageId::SettingsPopup },
        { SectionId::OnlineMulti, PageId::FriendRoomRules },
        { SectionId::OnlineMulti, PageId::MenuSettings },
        { SectionId::OnlineMulti, PageId::SettingsPopup },

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

        { SectionId::OnlineSingle, PageId::OnlineTop },

        { SectionId::OnlineMulti, PageId::OnlineTop },

        { SectionId::Voting1PVS, PageId::CourseSelect },

        { SectionId::VotingServer, PageId::Roulette },

        { SectionId::OnlineServer, PageId::FriendMatching },
    };
    for (const auto &addition : additions) {
        if (addition.first == id) {
            addActivePage(addition.second);
        }
    }
}

Page *Section::CreatePage(PageId pageId) {
    switch (pageId) {
    case PageId::ResultTeamVSTotal:
        return new ResultTeamVSTotalPage;
    case PageId::Award:
        return new AwardPage;
    case PageId::LicenseSelect:
        return new LicenseSelectPage;
    case PageId::SingleTop:
        return new SingleTopPage;
    case PageId::CourseSelect:
        return new CourseSelectPage;
    case PageId::TimeAttackGhostList:
        return new TimeAttackGhostListPage;
    case PageId::TeamConfirm:
        return new TeamConfirmPage;
    case PageId::MultiTop:
        return new MultiTopPage;
    case PageId::MultiTeamSelect:
        return new MultiTeamSelectPage;
    case PageId::DirectConnection:
        return new DirectConnectionPage;
    case PageId::OnlineTeamSelect:
        return new OnlineTeamSelectPage;
    case PageId::OnlineTop:
        return new OnlineTopPage;
    case PageId::VotingBack:
        return new VotingBackPage;
    case PageId::Roulette:
        return new RoulettePage;
    case PageId::FriendRoomRules:
        return new FriendRoomRulesPage;
    case PageId::FriendMatching:
        return new FriendMatchingPage;
    case PageId::FriendRoomBack:
        return new FriendRoomBackPage;
    case PageId::FriendRoom:
        return new FriendRoomPage;
    case PageId::FriendRoomMessageSelect:
        return new FriendRoomMessageSelectPage;
    case PageId::ServicePackTop:
        return new ServicePackTopPage;
    case PageId::StorageBenchmark:
        return new StorageBenchmarkPage;
    case PageId::ServicePackTools:
        return new ServicePackToolsPage;
    case PageId::GhostManager:
        return new GhostManagerPage;
    case PageId::Channel:
        return new ChannelPage;
    case PageId::Update:
        return new UpdatePage;
    case PageId::MenuSettings:
        return new MenuSettingsPage;
    case PageId::SettingsPopup:
        return new SettingsPagePopup;
    default:
        return REPLACED(CreatePage)(pageId);
    }
}

} // namespace UI
