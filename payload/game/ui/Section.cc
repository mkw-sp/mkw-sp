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
#include "game/ui/OnlineConnectionManagerPage.hh"
#include "game/ui/OnlineModeSelectPage.hh"
#include "game/ui/OnlineTeamSelectPage.hh"
#include "game/ui/OnlineTopPage.hh"
#include "game/ui/RandomMatchingPage.hh"
#include "game/ui/RoulettePage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/ServicePackToolsPage.hh"
#include "game/ui/ServicePackTopPage.hh"
#include "game/ui/SettingsPage.hh"
#include "game/ui/SingleTopPage.hh"
#include "game/ui/StorageBenchmarkPage.hh"
#include "game/ui/TeamConfirmPage.hh"
#include "game/ui/TimeAttackGhostListPage.hh"
#include "game/ui/UpdatePage.hh"
#include "game/ui/VotingBackPage.hh"
#include "game/ui/page/BattleModeSelectPage.hh"
#include "game/ui/page/DriftSelectPage.hh"
#include "game/ui/page/ResultTeamTotalPage.hh"

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

Vec2<f32> Section::scaleFor() const {
    return m_scaleFor;
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
    switch (sectionId) {
    case SectionId::OnlineSingle:
    case SectionId::OnlineMulti:
    case SectionId::Voting1PVS:
    case SectionId::Voting1PBalloon:
    case SectionId::Voting1PCoin:
    case SectionId::Voting2PVS:
    case SectionId::Voting2PBalloon:
    case SectionId::Voting2PCoin:
    case SectionId::OnlineFriend1PVS:
        return true;
    default:
        return false;
    }
}

bool Section::HasRaceClient(SectionId sectionId) {
    switch (sectionId) {
    case SectionId::OnlineFriend1PVS:
        return true;
    default:
        return false;
    }
}

void Section::addPage(PageId pageId) {
    std::pair<SectionId, PageId> deletions[] = {
            {SectionId::Battle1P, PageId::ResultBattleUpdate},
            {SectionId::Battle1P, PageId::ResultBattleTotal},
            {SectionId::Battle2P, PageId::ResultBattleUpdate},
            {SectionId::Battle2P, PageId::ResultBattleTotal},
            {SectionId::Battle3P, PageId::ResultBattleUpdate},
            {SectionId::Battle3P, PageId::ResultBattleTotal},
            {SectionId::Battle4P, PageId::ResultBattleUpdate},
            {SectionId::Battle4P, PageId::ResultBattleTotal},

            // The game has 5 pages for the records, we only need 1 for the settings. Remove the 4
            // others.
            {SectionId::LicenseSettings, PageId::SettingsPopup},
            {SectionId::LicenseSettings, PageId::LicenseRecordsFriends},
            {SectionId::LicenseSettings, PageId::LicenseRecordsWFC},
            {SectionId::LicenseSettings, PageId::LicenseRecordsOther},

            {SectionId::Single, PageId::VSSelect},
            {SectionId::Single, PageId::VSSetting},
            {SectionId::Single, PageId::BattleSetting},
            {SectionId::Single, PageId::RaceCourseSelect},
            {SectionId::Single, PageId::BattleCupSelect},
            {SectionId::Single, PageId::BattleCourseSelect},
            {SectionId::SingleSelectVSCourse, PageId::RaceCourseSelect},
            {SectionId::SingleSelectBTCourse, PageId::BattleCupSelect},
            {SectionId::SingleSelectBTCourse, PageId::BattleCourseSelect},
            {SectionId::SingleChangeDriver, PageId::VSSelect},
            {SectionId::SingleChangeDriver, PageId::VSSetting},
            {SectionId::SingleChangeDriver, PageId::BattleSetting},
            {SectionId::SingleChangeDriver, PageId::RaceCourseSelect},
            {SectionId::SingleChangeDriver, PageId::BattleCupSelect},
            {SectionId::SingleChangeDriver, PageId::BattleCourseSelect},
            {SectionId::SingleChangeCourse, PageId::VSSelect},
            {SectionId::SingleChangeCourse, PageId::VSSetting},
            {SectionId::SingleChangeCourse, PageId::BattleSetting},
            {SectionId::SingleChangeCourse, PageId::RaceCourseSelect},
            {SectionId::SingleChangeCourse, PageId::BattleCupSelect},
            {SectionId::SingleChangeCourse, PageId::BattleCourseSelect},
            {SectionId::SingleChangeMission, PageId::VSSelect},
            {SectionId::SingleChangeMission, PageId::VSSetting},
            {SectionId::SingleChangeMission, PageId::BattleSetting},
            {SectionId::SingleChangeMission, PageId::RaceCourseSelect},
            {SectionId::Multi, PageId::RaceCourseSelect},
            {SectionId::Multi, PageId::VSSelect},
            {SectionId::Multi, PageId::VSSetting},
            {SectionId::Multi, PageId::BattleSetting},
            {SectionId::Multi, PageId::BattleCupSelect},
            {SectionId::Multi, PageId::BattleCourseSelect},

            {SectionId::OnlineSingle, PageId::WifiFriendMenu},

            {SectionId::OnlineMulti, PageId::ConfirmWifiQuit},
            {SectionId::OnlineMulti, PageId::ReadingGhostData},
            {SectionId::OnlineMulti, PageId::ConnectingNintendoWfc},
            {SectionId::OnlineMulti, PageId::Confirm},
            {SectionId::OnlineMulti, PageId::CharacterSelect},
            {SectionId::OnlineMulti, PageId::BattleVehicleSelect},
            {SectionId::OnlineMulti, PageId::ModelRender},
            {SectionId::OnlineMulti, PageId::MultiVehicleSelect},
            {SectionId::OnlineMulti, PageId::MultiDriftSelect},
            {SectionId::OnlineMulti, PageId::WifiFirstPlay},
            {SectionId::OnlineMulti, PageId::WifiDataConsent},
            {SectionId::OnlineMulti, PageId::WifiDisconnect},
            {SectionId::OnlineMulti, PageId::WifiConnectionFailed},
            {SectionId::OnlineMulti, PageId::OnlineModeSelect},
            {SectionId::OnlineMulti, PageId::WifiFriendMenu},
            {SectionId::OnlineMulti, PageId::WifiFriendRoster},
            {SectionId::OnlineMulti, PageId::WifiNoFriendsPopup},
            {SectionId::OnlineMulti, PageId::WifiFriendRemoveConfirm},
            {SectionId::OnlineMulti, PageId::WifiFriendRemoving},
            {SectionId::OnlineMulti, PageId::UnknownA5},
            {SectionId::OnlineMulti, PageId::EnterFriendCode},
            {SectionId::OnlineMulti, PageId::GhostManager},

            {SectionId::Voting1PVS, PageId::ConnectingNintendoWfc},
            {SectionId::Voting1PVS, PageId::MenuMessage},
            {SectionId::Voting1PVS, PageId::RaceCourseSelect},
            {SectionId::Voting1PVS, PageId::OnlineConnectionManager},
            {SectionId::Voting1PVS, PageId::WifiPlayerList},

            {SectionId::OnlineFriend1PVS, PageId::Unknown44},
            {SectionId::OnlineFriend1PVS, PageId::OnlinePleaseWait},
            {SectionId::OnlineFriend1PVS, PageId::OnlineTeamSelect},
            {SectionId::OnlineFriend1PVS, PageId::OnlineConnectionManager},

            // The channel section is repurposed into the Service Pack section. Remove some pages
            // that aren't needed anymore.
            {SectionId::ServicePack, PageId::TimeAttackTop},
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
            {SectionId::SingleChangeGhostData, PageId::CharacterSelect},

            {SectionId::SingleSelectBTCourse, PageId::BattleCupSelect},
            {SectionId::SingleSelectBTCourse, PageId::BattleCourseSelect},

            {SectionId::OnlineSingle, PageId::DirectConnection},

            {SectionId::OnlineMulti, PageId::OnlineTeamSelect},
            {SectionId::OnlineMulti, PageId::GhostManager},
            {SectionId::OnlineMulti, PageId::ModelRender},
            {SectionId::OnlineMulti, PageId::DirectConnection},

            {SectionId::Voting1PVS, PageId::OnlineConnectionManager},
            {SectionId::Voting1PVS, PageId::OnlineTeamSelect},

            {SectionId::OnlineFriend1PVS, PageId::OnlineConnectionManager},
            {SectionId::OnlineFriend1PVS, PageId::OnlineTeamSelect},
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
            {SectionId::TA, PageId::ConfirmQuit},
            {SectionId::VS1P, PageId::ConfirmQuit},
            {SectionId::TeamVS1P, PageId::ConfirmQuit},
            {SectionId::Battle1P, PageId::ConfirmQuit},
            {SectionId::TAReplay, PageId::ConfirmQuit},

            {SectionId::VS1P, PageId::ResultTeamVSTotal},
            {SectionId::VS2P, PageId::ResultTeamVSTotal},
            {SectionId::VS3P, PageId::ResultTeamVSTotal},
            {SectionId::VS4P, PageId::ResultTeamVSTotal},

            {SectionId::Battle1P, PageId::ResultRaceUpdate},
            {SectionId::Battle1P, PageId::ResultRaceTotal},
            {SectionId::Battle1P, PageId::ResultTeamVSTotal},
            {SectionId::Battle2P, PageId::ResultRaceUpdate},
            {SectionId::Battle2P, PageId::ResultRaceTotal},
            {SectionId::Battle2P, PageId::ResultTeamVSTotal},
            {SectionId::Battle3P, PageId::ResultRaceUpdate},
            {SectionId::Battle3P, PageId::ResultRaceTotal},
            {SectionId::Battle3P, PageId::ResultTeamVSTotal},
            {SectionId::Battle4P, PageId::ResultRaceUpdate},
            {SectionId::Battle4P, PageId::ResultRaceTotal},
            {SectionId::Battle4P, PageId::ResultTeamVSTotal},

            // Support changing settings in-race
            {SectionId::GP, PageId::MenuSettings},
            {SectionId::TA, PageId::MenuSettings},
            {SectionId::VS1P, PageId::MenuSettings},
            {SectionId::VS2P, PageId::MenuSettings},
            {SectionId::VS3P, PageId::MenuSettings},
            {SectionId::VS4P, PageId::MenuSettings},
            {SectionId::TeamVS1P, PageId::MenuSettings},
            {SectionId::TeamVS2P, PageId::MenuSettings},
            {SectionId::TeamVS3P, PageId::MenuSettings},
            {SectionId::TeamVS4P, PageId::MenuSettings},
            {SectionId::Battle1P, PageId::MenuSettings},
            {SectionId::Battle2P, PageId::MenuSettings},
            {SectionId::Battle3P, PageId::MenuSettings},
            {SectionId::Battle4P, PageId::MenuSettings},
            {SectionId::MR, PageId::MenuSettings},
            {SectionId::TournamentReplay, PageId::MenuSettings},
            {SectionId::GPReplay, PageId::MenuSettings},
            {SectionId::TAReplay, PageId::MenuSettings},
            {SectionId::GhostTA, PageId::MenuSettings},
            {SectionId::GhostTAOnline, PageId::MenuSettings},
            {SectionId::GhostReplayChannel, PageId::MenuSettings},
            {SectionId::GhostReplayDownload, PageId::MenuSettings},
            {SectionId::GhostReplay, PageId::MenuSettings},

            // Mission Mode
            {SectionId::MR, PageId::CompetitionPersonalLeaderboard},
            {SectionId::SingleChangeMission, PageId::MissionLevelSelect},
            {SectionId::SingleChangeMission, PageId::MissionStageSelect},
            {SectionId::SingleChangeMission, PageId::MissionInstruction},
            {SectionId::SingleChangeMission, PageId::MissionDrift},
            {SectionId::SingleChangeMission, PageId::MissionTutorial},
            {SectionId::Single, PageId::MissionLevelSelect},
            {SectionId::Single, PageId::MissionStageSelect},
            {SectionId::Single, PageId::MissionInstruction},
            {SectionId::Single, PageId::MissionDrift},
            {SectionId::Single, PageId::MissionTutorial},
            {SectionId::SingleChangeDriver, PageId::MissionLevelSelect},
            {SectionId::SingleChangeDriver, PageId::MissionStageSelect},
            {SectionId::SingleChangeDriver, PageId::MissionInstruction},
            {SectionId::SingleChangeDriver, PageId::MissionDrift},
            {SectionId::SingleChangeDriver, PageId::MissionTutorial},
            {SectionId::SingleChangeCourse, PageId::MissionLevelSelect},
            {SectionId::SingleChangeCourse, PageId::MissionStageSelect},
            {SectionId::SingleChangeCourse, PageId::MissionInstruction},
            {SectionId::SingleChangeCourse, PageId::MissionDrift},
            {SectionId::SingleChangeCourse, PageId::MissionTutorial},
            {SectionId::SingleChangeGhostData, PageId::MissionLevelSelect},
            {SectionId::SingleChangeGhostData, PageId::MissionStageSelect},
            {SectionId::SingleChangeGhostData, PageId::MissionInstruction},
            {SectionId::SingleChangeGhostData, PageId::MissionDrift},
            {SectionId::SingleChangeGhostData, PageId::MissionTutorial},

            {SectionId::SingleSelectBTCourse, PageId::CourseSelect},
            {SectionId::SingleSelectBTCourse, PageId::GhostManager},

            // Change Ghost Data
            {SectionId::SingleChangeGhostData, PageId::ReadingGhostData},
            {SectionId::SingleChangeGhostData, PageId::MenuMessage},
            {SectionId::SingleChangeGhostData, PageId::MessageBoardPopup},
            {SectionId::SingleChangeGhostData, PageId::SingleTop},
            {SectionId::SingleChangeGhostData, PageId::GpClassSelect},
            {SectionId::SingleChangeGhostData, PageId::CourseSelect},
            {SectionId::SingleChangeGhostData, PageId::TimeAttackTop},
            {SectionId::SingleChangeGhostData, PageId::TimeAttackGhostList},
            {SectionId::SingleChangeGhostData, PageId::TeamConfirm},
            {SectionId::SingleChangeGhostData, PageId::BattleModeSelect},
            {SectionId::SingleChangeGhostData, PageId::BattleVehicleSelect},

            {SectionId::Single, PageId::MenuSettings},
            {SectionId::SingleChangeDriver, PageId::MenuSettings},
            {SectionId::SingleChangeCourse, PageId::MenuSettings},
            {SectionId::SingleChangeGhostData, PageId::MenuSettings},
            {SectionId::Multi, PageId::MenuSettings},

            {SectionId::OnlineSingle, PageId::FriendRoomRules},
            {SectionId::OnlineSingle, PageId::MenuSettings},
            {SectionId::OnlineSingle, PageId::SettingsPopup},
            {SectionId::OnlineMulti, PageId::FriendRoomRules},
            {SectionId::OnlineMulti, PageId::MenuSettings},
            {SectionId::OnlineMulti, PageId::SettingsPopup},

            // The channel section is repurposed into the Service Pack section. Add some additional
            // pages we need.
            {SectionId::ServicePack, PageId::OptionExplanation},
            {SectionId::ServicePack, PageId::OptionSelect2},
            {SectionId::ServicePack, PageId::OptionAwait},
            {SectionId::ServicePack, PageId::OptionMessage},
            {SectionId::ServicePack, PageId::OptionConfirm},
            {SectionId::ServicePack, PageId::Update},
            {SectionId::ServicePack, PageId::Channel},
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
            {SectionId::SingleChangeGhostData, PageId::TimeAttackTop},
            {SectionId::SingleSelectBTCourse, PageId::CourseSelect},

            // Mission Mode
            {SectionId::SingleChangeMission, PageId::MissionLevelSelect},

            {SectionId::OnlineSingle, PageId::OnlineTop},

            {SectionId::OnlineMulti, PageId::OnlineTop},

            {SectionId::Voting1PVS, PageId::CourseSelect},
            {SectionId::Voting1PVS, PageId::OnlineConnectionManager},
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
        return new ResultTeamTotalPage;
    case PageId::AwardInterface:
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
    case PageId::OnlineConnectionManager:
        return new OnlineConnectionManagerPage;
    case PageId::OnlineTeamSelect:
        return new OnlineTeamSelectPage;
    case PageId::OnlineTop:
        return new OnlineTopPage;
    case PageId::OnlineModeSelect:
        return new OnlineModeSelectPage;
    case PageId::RandomMatching:
        return new RandomMatchingPage;
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

bool Section::logPageInfo(Page *page) {
    if (page == nullptr) {
        return false;
    }

    auto pageId = page->id();
    auto pageName = magic_enum::enum_name<PageId>(pageId);

    OSReport("    %s (0x%x)\n", pageName.data(), static_cast<u32>(pageId));
    return true;
}

void Section::logDebuggingInfo(bool verbose) {
    auto sectionManager = SectionManager::Instance();
    auto lastSectionId = sectionManager->nextSectionId();
    auto nextSectionId = sectionManager->nextSectionId();

    auto sectionName = magic_enum::enum_name<SectionId>(m_id);
    auto lastSectionName = magic_enum::enum_name<SectionId>(lastSectionId);
    auto nextSectionName = magic_enum::enum_name<SectionId>(nextSectionId);

    OSReport("Last Section: %s (0x%x)\n", lastSectionName.data(), static_cast<u32>(lastSectionId));
    OSReport("Current Section: %s (0x%x)\n", sectionName.data(), static_cast<u32>(m_id));
    OSReport("Next Section: %s (0x%x)\n", nextSectionName.data(), static_cast<u32>(nextSectionId));

    OSReport("Active Pages:\n");
    for (auto page : m_activePages) {
        if (!logPageInfo(page)) {
            break;
        }
    }

    if (!verbose) {
        return;
    }

    OSReport("Loaded Pages:\n");
    for (u16 i = 0; i < static_cast<u16>(PageId::Max); i += 1) {
        logPageInfo(m_pages[i]);
    }
}

} // namespace UI
