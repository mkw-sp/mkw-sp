#pragma once

#include "game/ui/Page.hh"
#include "game/ui/SectionId.hh"

#include "game/host_system/ContextId.hh"
#include "game/host_system/Scene.hh"
#include "game/sound/GroupId.hh"

#include <nw4r/lyt/lyt_drawInfo.hh>

namespace UI {

class BattleModeSelectPage;
class ConfirmPage;
class CourseSelectPage;
class DriftSelectPage;
class FriendMatchingPage;
class FriendRoomBackPage;
class FriendRoomMessageSelectPage;
class FriendRoomPage;
class FriendRoomRulesPage;
class GhostManagerPage;
class GlobePage;
class MenuAwaitPage;
class MenuMessagePage;
class MenuSettingsPage;
class MessagePagePopup;
class MissionInstructionPage;
class ModelPage;
class ModelRenderPage;
class OnlineConnectionManagerPage;
class OnlineModeSelectPage;
class OnlineTeamSelectPage;
class OptionExplanationPage;
class OptionSelectPage;
class RaceConfirmPage;
class RankingPage;
class RoulettePage;
class SPRankingGhostDownloadPage;
class SPRankingTopTenDownloadPage;
class SettingsPagePopup;
class TeamConfirmPage;
class TimeAttackGhostListPage;
class VotingBackPage;
class YesNoPagePopup;

class Section {
private:
    template <PageId P>
    struct PageIdHelper;

public:
    Section();

    Page *page(PageId pageId);

    template <PageId P>
    PageIdHelper<P>::type *page() {
        return reinterpret_cast<PageIdHelper<P>::type *>(m_pages[static_cast<size_t>(P)]);
    }

    SectionId id() const {
        return m_id;
    }
    bool isPageFocused(const Page *page) const;
    bool isPageActive(PageId pageId) const;
    Vec2<f32> scaleFor() const;
    f32 locationAdjustScaleDivider() const;
    void loadTHP();
    void loadFriendListManager();
    void pullPage();

    void logDebuggingInfo(bool verbose);

    static System::SceneId REPLACED(GetSceneId)(SectionId id);
    static REPLACE System::SceneId GetSceneId(SectionId id);
    static System::SceneId HandleSceneIdPatches(SectionId id);
    static const char *REPLACED(GetResourceName)(SectionId id);
    static REPLACE const char *GetResourceName(SectionId id);
    static const char *HandleResourceNamePatches(SectionId id);
    static bool REPLACED(HasBackModel)(SectionId id);
    static REPLACE bool HasBackModel(SectionId id);
    static System::ContextId REPLACED(GetContextId)(const SectionId id);
    static REPLACE System::ContextId GetContextId(const SectionId id);

    static Sound::SoundId REPLACED(GetSoundId)(const SectionId id);
    static REPLACE Sound::SoundId GetSoundId(const SectionId id);
    static Sound::GroupId REPLACED(GetGroupId)(const SectionId id);
    static REPLACE Sound::GroupId GetGroupId(const SectionId id);

    static s32 REPLACED(GetPriority)(const SectionId id);
    static REPLACE s32 GetPriority(const SectionId id);
    static s32 REPLACED(GetSoundTrigger)(const PageId id);
    static REPLACE s32 GetSoundTrigger(const PageId id);

    static bool HasRoomClient(SectionId sectionId);
    static bool HasRaceClient(SectionId sectionId);
    static bool HasOnlineManager(SectionId sectionId);

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

    static bool logPageInfo(Page *page);

public:
    REPLACE void init(SectionId id);
    REPLACE void deinit();

private:
    void REPLACED(init)(SectionId id);
    void REPLACED(deinit)();

    void popActivePages(s32);

    SectionId m_id;
    u8 _004[0x008 - 0x004];
    std::array<Page *, StandardPageCount()> m_pages;
    Page *m_activePages[10];
    u32 m_activePageCount;
    std::array<Page *, 2> m_systemPages;
    u8 _388[0x390 - 0x388];
    nw4r::lyt::DrawInfo m_drawInfo;
    u8 _3e4[0x3f8 - 0x3e4];
    Vec2<f32> m_scaleFor;
    u8 _400[0x408 - 0x400];
    std::array<Page *, ExtendedPageCount()> m_pagesEXT{};
};
static_assert(sizeof(Section) ==
        ROUND_UP(0x408 + std::max(1uz, sizeof(Page *) * ExtendedPageCount()), alignof(Section)));

template <>
struct Section::PageIdHelper<PageId::RaceConfirm> {
    using type = RaceConfirmPage;
};

template <>
struct Section::PageIdHelper<PageId::MessagePopup> {
    using type = MessagePagePopup;
};

template <>
struct Section::PageIdHelper<PageId::YesNoPopup> {
    using type = YesNoPagePopup;
};

template <>
struct Section::PageIdHelper<PageId::SpinnerAwait> {
    using type = MenuAwaitPage;
};

template <>
struct Section::PageIdHelper<PageId::MenuMessage> {
    using type = MenuMessagePage;
};

template <>
struct Section::PageIdHelper<PageId::Confirm> {
    using type = ConfirmPage;
};

template <>
struct Section::PageIdHelper<PageId::Model> {
    using type = ModelPage;
};

template <>
struct Section::PageIdHelper<PageId::DriftSelect> {
    using type = DriftSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::CourseSelect> {
    using type = CourseSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::TimeAttackGhostList> {
    using type = TimeAttackGhostListPage;
};

template <>
struct Section::PageIdHelper<PageId::TeamConfirm> {
    using type = TeamConfirmPage;
};

template <>
struct Section::PageIdHelper<PageId::MissionInstruction> {
    using type = MissionInstructionPage;
};

template <>
struct Section::PageIdHelper<PageId::ModelRender> {
    using type = ModelRenderPage;
};

template <>
struct Section::PageIdHelper<PageId::OnlineTeamSelect> {
    using type = OnlineTeamSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::VotingBack> {
    using type = VotingBackPage;
};

template <>
struct Section::PageIdHelper<PageId::Roulette> {
    using type = RoulettePage;
};

template <>
struct Section::PageIdHelper<PageId::Globe> {
    using type = GlobePage;
};

template <>
struct Section::PageIdHelper<PageId::OnlineConnectionManager> {
    using type = OnlineConnectionManagerPage;
};

template <>
struct Section::PageIdHelper<PageId::FriendMatching> {
    using type = FriendMatchingPage;
};

template <>
struct Section::PageIdHelper<PageId::FriendRoomRules> {
    using type = FriendRoomRulesPage;
};

template <>
struct Section::PageIdHelper<PageId::FriendRoomBack> {
    using type = FriendRoomBackPage;
};

template <>
struct Section::PageIdHelper<PageId::FriendRoom> {
    using type = FriendRoomPage;
};

template <>
struct Section::PageIdHelper<PageId::FriendRoomMessageSelect> {
    using type = FriendRoomMessageSelectPage;
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
struct Section::PageIdHelper<PageId::MenuSettings> {
    using type = MenuSettingsPage;
};

template <>
struct Section::PageIdHelper<PageId::SettingsPopup> {
    using type = SettingsPagePopup;
};

template <>
struct Section::PageIdHelper<PageId::BattleModeSelect> {
    using type = BattleModeSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::OnlineModeSelect> {
    using type = OnlineModeSelectPage;
};

template <>
struct Section::PageIdHelper<PageId::Ranking> {
    using type = RankingPage;
};

template <>
struct Section::PageIdHelper<PageId::SPRankingGhostDownload> {
    using type = SPRankingGhostDownloadPage;
};

template <>
struct Section::PageIdHelper<PageId::SPRankingTopTenDownload> {
    using type = SPRankingTopTenDownloadPage;
};

} // namespace UI
