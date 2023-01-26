#include "ClientSettings.hh"

#include <iterator>

namespace SP::ClientSettings {

const char name[] = "MKW-SP Settings";

const u32 categoryMessageIds[] = { 10118, 10126, 10119, 10262, 10258, 10120, 10315 };

const Entry entries[] = {
    [static_cast<u32>(Setting::DriftMode)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::DriftMode),
        .messageId = 0,
        .defaultValue = static_cast<u32>(DriftMode::Manual),
        .valueCount = magic_enum::enum_count<DriftMode>(),
        .valueNames = magic_enum::enum_names<DriftMode>().data(),
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
        .hidden = true,
    },
    [static_cast<u32>(Setting::VanillaMode)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::VanillaMode),
        .messageId = 10166,
        .defaultValue = static_cast<u32>(VanillaMode::Disable),
        .valueCount = magic_enum::enum_count<VanillaMode>(),
        .valueNames = magic_enum::enum_names<VanillaMode>().data(),
        .valueMessageIds = (u32[]) { 10167, 10168 },
        .valueExplanationMessageIds = (u32[]) { 10169, 10170 },
    },
    [static_cast<u32>(Setting::SimplifiedControls)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::SimplifiedControls),
        .messageId = 10301,
        .defaultValue = static_cast<u32>(SimplifiedControls::Off),
        .valueCount = magic_enum::enum_count<SimplifiedControls>(),
        .valueNames = magic_enum::enum_names<SimplifiedControls>().data(),
        .valueMessageIds = (u32[]) { 10302, 10303, 10304 },
        .valueExplanationMessageIds = (u32[]) { 10305, 10306, 10307 },
    },
    [static_cast<u32>(Setting::FOV169)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::FOV169),
        .messageId = 10009,
        .defaultValue = static_cast<u32>(FOV169::FOV169),
        .valueCount = magic_enum::enum_count<FOV169>(),
        .valueNames = magic_enum::enum_names<FOV169>().data(),
        .valueMessageIds = (u32[]) { 10010, 10011 },
        .valueExplanationMessageIds = (u32[]) { 10012, 10013 },
        .vanillaValue = static_cast<u32>(FOV169::FOV169),
    },
    [static_cast<u32>(Setting::RegionLineColor)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::RegionLineColor),
        .messageId = 10194,
        .defaultValue = static_cast<u32>(RegionLineColor::Default),
        .valueCount = magic_enum::enum_count<RegionLineColor>(),
        .valueNames = magic_enum::enum_names<RegionLineColor>().data(),
        .valueMessageIds = (u32[]) { 10195, 10196, 10197, 10198, 10199, 10200, 10332 },
        .valueExplanationMessageIds = (u32[]) { 10201, 10202, 10203, 10204, 10205, 10206, 10333 },
        .vanillaValue = static_cast<u32>(RegionLineColor::Default),
    },
    [static_cast<u32>(Setting::FarPlayerTags)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::FarPlayerTags),
        .messageId = 10308,
        .defaultValue = static_cast<u32>(FarPlayerTags::Disable),
        .valueCount = magic_enum::enum_count<FarPlayerTags>(),
        .valueNames = magic_enum::enum_names<FarPlayerTags>().data(),
        .valueMessageIds = (u32[]) { 10309, 10310 },
        .valueExplanationMessageIds = (u32[]) { 10311, 10312 },
        .vanillaValue = static_cast<u32>(FarPlayerTags::Disable),
    },
    [static_cast<u32>(Setting::PlayerTags)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::PlayerTags),
        .messageId = 10183,
        .defaultValue = 3,
        .valueCount = 13,
        .valueNames = nullptr,
        .valueMessageIds = (u32[]) { 10184 },
        .valueExplanationMessageIds = (u32[]) { 10185 },
        .vanillaValue = 3,
    },
    [static_cast<u32>(Setting::HUDLabels)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::HUDLabels),
        .messageId = 10004,
        .defaultValue = static_cast<u32>(HUDLabels::Show),
        .valueCount = magic_enum::enum_count<HUDLabels>(),
        .valueNames = magic_enum::enum_names<HUDLabels>().data(),
        .valueMessageIds = (u32[]) { 10005, 10006 },
        .valueExplanationMessageIds = (u32[]) { 10007, 10008 },
        .vanillaValue = static_cast<u32>(HUDLabels::Show),
    },
    [static_cast<u32>(Setting::HUDTeamColors)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::HUDTeamColors),
        .messageId = 10296,
        .defaultValue = static_cast<u32>(HUDTeamColors::Enable),
        .valueCount = magic_enum::enum_count<HUDTeamColors>(),
        .valueNames = magic_enum::enum_names<HUDTeamColors>().data(),
        .valueMessageIds = (u32[]) { 10297, 10298 },
        .valueExplanationMessageIds = (u32[]) { 10299, 10300 },
    },
    [static_cast<u32>(Setting::MiniMap)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::MiniMap),
        .messageId = 10367,
        .defaultValue = static_cast<u32>(MiniMap::Enable),
        .valueCount = magic_enum::enum_count<MiniMap>(),
        .valueNames = magic_enum::enum_names<MiniMap>().data(),
        .valueMessageIds = (u32[]) { 10368, 10369 },
        .valueExplanationMessageIds = (u32[]) { 10370, 10371 },
        .vanillaValue = static_cast<u32>(MiniMap::Enable),
    },
    [static_cast<u32>(Setting::MapIcons)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::MapIcons),
        .messageId = 10014,
        .defaultValue = static_cast<u32>(MapIcons::Miis),
        .valueCount = magic_enum::enum_count<MapIcons>(),
        .valueNames = magic_enum::enum_names<MapIcons>().data(),
        .valueMessageIds = (u32[]) { 10015, 10016 },
        .valueExplanationMessageIds = (u32[]) { 10017, 10018 },
        .vanillaValue = static_cast<u32>(MapIcons::Characters),
    },
    [static_cast<u32>(Setting::InputDisplay)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::InputDisplay),
        .messageId = 10061,
        .defaultValue = static_cast<u32>(InputDisplay::Simple),
        .valueCount = magic_enum::enum_count<InputDisplay>(),
        .valueNames = magic_enum::enum_names<InputDisplay>().data(),
        .valueMessageIds = (u32[]) { 10062, 10063 },
        .valueExplanationMessageIds = (u32[]) { 10064, 10065 },
        .vanillaValue = static_cast<u32>(InputDisplay::Disable),
    },
    [static_cast<u32>(Setting::Speedometer)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::Speedometer),
        .messageId = 10321,
        .defaultValue = static_cast<u32>(Speedometer::InternalPlus),
        .valueCount = magic_enum::enum_count<Speedometer>(),
        .valueNames = magic_enum::enum_names<Speedometer>().data(),
        .valueMessageIds = (u32[]) { 10322, 10323, 10324, 10325, 10326 },
        .valueExplanationMessageIds = (u32[]) { 10327, 10328, 10329, 10330, 10331 },
    },
    [static_cast<u32>(Setting::RankControl)] = {
        .category = Category::Race,
        .name = magic_enum::enum_name(Setting::RankControl),
        .messageId = 10121,
        .defaultValue = static_cast<u32>(RankControl::GPVS),
        .valueCount = magic_enum::enum_count<RankControl>(),
        .valueNames = magic_enum::enum_names<RankControl>().data(),
        .valueMessageIds = (u32[]) { 10122, 10123 },
        .valueExplanationMessageIds = (u32[]) { 10124, 10125 },
        .vanillaValue = static_cast<u32>(RankControl::GPVS),
    },
    [static_cast<u32>(Setting::ItemMusic)] = {
        .category = Category::Sound,
        .name = magic_enum::enum_name(Setting::ItemMusic),
        .messageId = 10127,
        .defaultValue = static_cast<u32>(ItemMusic::All),
        .valueCount = magic_enum::enum_count<ItemMusic>(),
        .valueNames = magic_enum::enum_names<ItemMusic>().data(),
        .valueMessageIds = (u32[]) { 10128, 10129, 10130 },
        .valueExplanationMessageIds = (u32[]) { 10131, 10132, 10133 },
        .vanillaValue = static_cast<u32>(ItemMusic::All),
    },
    [static_cast<u32>(Setting::LastLapJingle)] = {
        .category = Category::Sound,
        .name = magic_enum::enum_name(Setting::LastLapJingle),
        .messageId = 10134,
        .defaultValue = static_cast<u32>(LastLapJingle::GameplayOnly),
        .valueCount = magic_enum::enum_count<LastLapJingle>(),
        .valueNames = magic_enum::enum_names<LastLapJingle>().data(),
        .valueMessageIds = (u32[]) { 10135, 10136 },
        .valueExplanationMessageIds = (u32[]) { 10137, 10138 },
        .vanillaValue = static_cast<u32>(LastLapJingle::GameplayOnly),
    },
    [static_cast<u32>(Setting::LastLapSpeedup)] = {
        .category = Category::Sound,
        .name = magic_enum::enum_name(Setting::LastLapSpeedup),
        .messageId = 10139,
        .defaultValue = static_cast<u32>(LastLapSpeedup::Static),
        .valueCount = magic_enum::enum_count<LastLapSpeedup>(),
        .valueNames = magic_enum::enum_names<LastLapSpeedup>().data(),
        .valueMessageIds = (u32[]) { 10140, 10141, 10142 },
        .valueExplanationMessageIds = (u32[]) { 10143, 10144, 10145 },
        .vanillaValue = static_cast<u32>(LastLapSpeedup::Static),
    },
    [static_cast<u32>(Setting::TAClass)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TAClass),
        .messageId = 3410,
        .defaultValue = static_cast<u32>(TAClass::CC150),
        .valueCount = magic_enum::enum_count<TAClass>(),
        .valueNames = magic_enum::enum_names<TAClass>().data(),
        .valueMessageIds = (u32[]) { 3413, 10072 },
        .valueExplanationMessageIds = (u32[]) { 10079, 10081 },
    },
    [static_cast<u32>(Setting::TAGhostSorting)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TAGhostSorting),
        .messageId = 10019,
        .defaultValue = static_cast<u32>(TAGhostSorting::Time),
        .valueCount = magic_enum::enum_count<TAGhostSorting>(),
        .valueNames = magic_enum::enum_names<TAGhostSorting>().data(),
        .valueMessageIds = (u32[]) { 10171, 10172, 10173, 10174, 10175 },
        .valueExplanationMessageIds = (u32[]) { 10176, 10177, 10178, 10179, 10180 },
    },
    [static_cast<u32>(Setting::TAGhostTagVisibility)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TAGhostTagVisibility),
        .messageId = 10028,
        .defaultValue = static_cast<u32>(TAGhostTagVisibility::All),
        .valueCount = magic_enum::enum_count<TAGhostTagVisibility>(),
        .valueNames = magic_enum::enum_names<TAGhostTagVisibility>().data(),
        .valueMessageIds = (u32[]) { 10029, 10030, 10031 },
        .valueExplanationMessageIds = (u32[]) { 10032, 10033, 10034 },
        .vanillaValue = static_cast<u32>(TAGhostTagVisibility::Watched),
    },
    [static_cast<u32>(Setting::TAGhostTagContent)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TAGhostTagContent),
        .messageId = 10035,
        .defaultValue = static_cast<u32>(TAGhostTagContent::Name),
        .valueCount = magic_enum::enum_count<TAGhostTagContent>(),
        .valueNames = magic_enum::enum_names<TAGhostTagContent>().data(),
        .valueMessageIds = (u32[]) { 10036, 10037, 10053, 10038 },
        .valueExplanationMessageIds = (u32[]) { 10039, 10040, 10054, 10041 },
        .vanillaValue = static_cast<u32>(TAGhostTagContent::Name),
    },
    [static_cast<u32>(Setting::TASolidGhosts)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TASolidGhosts),
        .messageId = 10042,
        .defaultValue = static_cast<u32>(TASolidGhosts::None),
        .valueCount = magic_enum::enum_count<TASolidGhosts>(),
        .valueNames = magic_enum::enum_names<TASolidGhosts>().data(),
        .valueMessageIds = (u32[]) { 10029, 10030, 10031 },
        .valueExplanationMessageIds = (u32[]) { 10043, 10044, 10045 },
        .vanillaValue = static_cast<u32>(TASolidGhosts::None),
    },
    [static_cast<u32>(Setting::TAGhostSound)] = {
        .category = Category::TA,
        .name = magic_enum::enum_name(Setting::TAGhostSound),
        .messageId = 10066,
        .defaultValue = static_cast<u32>(TAGhostSound::Watched),
        .valueCount = magic_enum::enum_count<TAGhostSound>(),
        .valueNames = magic_enum::enum_names<TAGhostSound>().data(),
        .valueMessageIds = (u32[]) { 10029, 10030, 10031 },
        .valueExplanationMessageIds = (u32[]) { 10067, 10068, 10069 },
        .vanillaValue = static_cast<u32>(TAGhostSound::Watched),
    },
    [static_cast<u32>(Setting::VSTeamSize)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSTeamSize),
        .messageId = 10207,
        .defaultValue = static_cast<u32>(VSTeamSize::FFA),
        .valueCount = magic_enum::enum_count<VSTeamSize>(),
        .valueNames = magic_enum::enum_names<VSTeamSize>().data(),
        .valueMessageIds = (u32[]) { 10208, 10209, 10210, 10211, 10212 },
        .valueExplanationMessageIds = (u32[]) { 10213, 10214, 10215, 10216, 10217 },
    },
    [static_cast<u32>(Setting::VSRaceCount)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSRaceCount),
        .messageId = 10224,
        .defaultValue = 4,
        .valueCount = 32,
        .valueOffset = 1,
        .valueNames = nullptr,
        .valueMessageIds = (u32[]) { 10225 },
        .valueExplanationMessageIds = (u32[]) { 10226 },
    },
    [static_cast<u32>(Setting::VSCourseSelection)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSCourseSelection),
        .messageId = 10227,
        .defaultValue = static_cast<u32>(VSCourseSelection::Choose),
        .valueCount = magic_enum::enum_count<VSCourseSelection>(),
        .valueNames = magic_enum::enum_names<VSCourseSelection>().data(),
        .valueMessageIds = (u32[]) { 3441, 10228, 3443 },
        .valueExplanationMessageIds = (u32[]) { 10260, 10231, 10261 },
    },
    [static_cast<u32>(Setting::VSClass)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSClass),
        .messageId = 10234,
        .defaultValue = static_cast<u32>(VSClass::Mixed),
        .valueCount = magic_enum::enum_count<VSClass>(),
        .valueNames = magic_enum::enum_names<VSClass>().data(),
        .valueMessageIds = (u32[]) { 10235, 10236, 10237, 10238 },
        .valueExplanationMessageIds = (u32[]) { 10239, 10240, 10241, 10242 },
    },
    [static_cast<u32>(Setting::VSVehicles)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSVehicles),
        .messageId = 10243,
        .defaultValue = static_cast<u32>(VSVehicles::All),
        .valueCount = magic_enum::enum_count<VSVehicles>(),
        .valueNames = magic_enum::enum_names<VSVehicles>().data(),
        .valueMessageIds = (u32[]) { 10244, 10245, 10246, 10247, 10248, 10249, 10250 },
        .valueExplanationMessageIds = (u32[]) { 10251, 10252, 10253, 10254, 10255, 10256, 10257 },
    },
    [static_cast<u32>(Setting::VSMegaClouds)] = {
        .category = Category::VS,
        .name = magic_enum::enum_name(Setting::VSMegaClouds),
        .messageId = 20035,
        .defaultValue = static_cast<u32>(VSMegaClouds::Disable),
        .valueCount = magic_enum::enum_count<VSMegaClouds>(),
        .valueNames = magic_enum::enum_names<VSMegaClouds>().data(),
        .valueMessageIds = (u32[]) { 20038, 20039 },
        .valueExplanationMessageIds = (u32[]) { 20036, 20037 },
    },
    [static_cast<u32>(Setting::RoomTeamSize)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomTeamSize),
        .messageId = 10207,
        .defaultValue = static_cast<u32>(RoomTeamSize::FFA),
        .valueCount = magic_enum::enum_count<RoomTeamSize>(),
        .valueNames = magic_enum::enum_names<RoomTeamSize>().data(),
        .valueMessageIds = (u32[]) { 10208, 10209, 10210, 10211, 10212 },
        .valueExplanationMessageIds = (u32[]) { 10213, 10214, 10215, 10216, 10217 },
    },
    [static_cast<u32>(Setting::RoomTeamSelection)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomTeamSelection),
        .messageId = 10259,
        .defaultValue = static_cast<u32>(RoomTeamSelection::Random),
        .valueCount = magic_enum::enum_count<RoomTeamSelection>(),
        .valueNames = magic_enum::enum_names<RoomTeamSelection>().data(),
        .valueMessageIds = (u32[]) { 10218, 10219, 10220 },
        .valueExplanationMessageIds = (u32[]) { 10221, 10222, 10223 },
    },
    [static_cast<u32>(Setting::RoomRaceCount)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomRaceCount),
        .messageId = 10224,
        .defaultValue = 4,
        .valueCount = 32,
        .valueOffset = 1,
        .valueNames = nullptr,
        .valueMessageIds = (u32[]) { 10225 },
        .valueExplanationMessageIds = (u32[]) { 10226 },
    },
    [static_cast<u32>(Setting::RoomCourseSelection)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomCourseSelection),
        .messageId = 10227,
        .defaultValue = static_cast<u32>(RoomCourseSelection::Random),
        .valueCount = magic_enum::enum_count<RoomCourseSelection>(),
        .valueNames = magic_enum::enum_names<RoomCourseSelection>().data(),
        .valueMessageIds = (u32[]) { 10228, 3443, 10229, 10230 },
        .valueExplanationMessageIds = (u32[]) { 10231, 10261, 10232, 10233 },
    },
    [static_cast<u32>(Setting::RoomClass)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomClass),
        .messageId = 10234,
        .defaultValue = static_cast<u32>(RoomClass::Mixed),
        .valueCount = magic_enum::enum_count<RoomClass>(),
        .valueNames = magic_enum::enum_names<RoomClass>().data(),
        .valueMessageIds = (u32[]) { 10235, 10236, 10237, 10238 },
        .valueExplanationMessageIds = (u32[]) { 10239, 10240, 10241, 10242 },
    },
    [static_cast<u32>(Setting::RoomVehicles)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomVehicles),
        .messageId = 10243,
        .defaultValue = static_cast<u32>(RoomVehicles::All),
        .valueCount = magic_enum::enum_count<RoomVehicles>(),
        .valueNames = magic_enum::enum_names<RoomVehicles>().data(),
        .valueMessageIds = (u32[]) { 10244, 10245, 10246, 10247, 10248, 10249, 10250 },
        .valueExplanationMessageIds = (u32[]) { 10251, 10252, 10253, 10254, 10255, 10256, 10257 },
    },
    [static_cast<u32>(Setting::RoomCodeHigh)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomCodeHigh),
        .messageId = 0,
        .defaultValue = std::numeric_limits<u32>::max(),
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::RoomCodeLow)] = {
        .category = Category::Room,
        .name = magic_enum::enum_name(Setting::RoomCodeLow),
        .messageId = 0,
        .defaultValue = std::numeric_limits<u32>::max(),
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::MiiAvatar)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::MiiAvatar),
        .messageId = 0,
        .defaultValue = 0x80000001,
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::MiiClient)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::MiiClient),
        .messageId = 0,
        .defaultValue = 0xECFF82D2,
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::ColorPalette)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::ColorPalette),
        .messageId = 10263,
        .defaultValue = static_cast<u32>(ColorPalette::Vivid),
        .valueCount = magic_enum::enum_count<ColorPalette>(),
        .valueNames = magic_enum::enum_names<ColorPalette>().data(),
        .valueMessageIds = (u32[]) { 10264, 10265 },
        .valueExplanationMessageIds = (u32[]) { 10266, 10267 },
    },
    [static_cast<u32>(Setting::LoadingScreenColor)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::LoadingScreenColor),
        .messageId = 0,
        .defaultValue = 0xFF,
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::GCPadRumble)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::GCPadRumble),
        .messageId = 10191,
        .defaultValue = static_cast<u32>(GCPadRumble::Enable),
        .valueCount = magic_enum::enum_count<GCPadRumble>(),
        .valueNames = magic_enum::enum_names<GCPadRumble>().data(),
        .valueMessageIds = (u32[]) { 10057, 10058 },
        .valueExplanationMessageIds = (u32[]) { 10192, 10193 },
    },
    [static_cast<u32>(Setting::PageTransitions)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::PageTransitions),
        .messageId = 10056,
        .defaultValue = static_cast<u32>(PageTransitions::Enable),
        .valueCount = magic_enum::enum_count<PageTransitions>(),
        .valueNames = magic_enum::enum_names<PageTransitions>().data(),
        .valueMessageIds = (u32[]) { 10057, 10058 },
        .valueExplanationMessageIds = (u32[]) { 10059, 10060 },
    },
    [static_cast<u32>(Setting::PerfOverlay)] = {
        .category = Category::DebugOverlay,
        .name = magic_enum::enum_name(Setting::PerfOverlay),
        .messageId = 10188,
        .defaultValue = static_cast<u32>(PerfOverlay::Disable),
        .valueCount = magic_enum::enum_count<PerfOverlay>(),
        .valueNames = magic_enum::enum_names<PerfOverlay>().data(),
        .valueMessageIds = (u32[]) { 10337, 10338 },
        .valueExplanationMessageIds = (u32[]) { 10189, 10190 },
    },
    [static_cast<u32>(Setting::RegionFlagDisplay)] = {
        .category = Category::License,
        .name = magic_enum::enum_name(Setting::RegionFlagDisplay),
        .messageId = 10334,
        .defaultValue = static_cast<u32>(RegionFlagDisplay::Enable),
        .valueCount = magic_enum::enum_count<RegionFlagDisplay>(),
        .valueNames = magic_enum::enum_names<RegionFlagDisplay>().data(),
        .valueMessageIds = (u32[]) { 10337, 10338 },
        .valueExplanationMessageIds = (u32[]) { 10335, 10336 },
    },
    [static_cast<u32>(Setting::DebugCheckpoints)] = {
        .category = Category::DebugOverlay,
        .name = magic_enum::enum_name(Setting::DebugCheckpoints),
        .messageId = 10316,
        .defaultValue = static_cast<u32>(DebugCheckpoints::Disable),
        .valueCount = magic_enum::enum_count<DebugCheckpoints>(),
        .valueNames = magic_enum::enum_names<DebugCheckpoints>().data(),
        .valueMessageIds = (u32[]) { 10317, 10318 },
        .valueExplanationMessageIds = (u32[]) { 10319, 10320 },
    },
    [static_cast<u32>(Setting::DebugPanel)] = {
        .category = Category::DebugOverlay,
        .name = magic_enum::enum_name(Setting::DebugPanel),
        .messageId = 10341,
        .defaultValue = static_cast<u32>(DebugPanel::Disable),
        .valueCount = magic_enum::enum_count<DebugPanel>(),
        .valueNames = magic_enum::enum_names<DebugPanel>().data(),
        .valueMessageIds = (u32[]) { 10342, 10343, 10344 },
        .valueExplanationMessageIds = (u32[]) { 10345, 10346, 10347 },
    },
    [static_cast<u32>(Setting::DebugKCL)] = {
        .category = Category::DebugOverlay,
        .name = magic_enum::enum_name(Setting::DebugKCL),
        .messageId = 10348,
        .defaultValue = static_cast<u32>(DebugKCL::Disable),
        .valueCount = magic_enum::enum_count<DebugKCL>(),
        .valueNames = magic_enum::enum_names<DebugKCL>().data(),
        .valueMessageIds = (u32[]) { 10349, 10351, 10353 },
        .valueExplanationMessageIds = (u32[]) { 10350, 10352, 10354 },
    },
};

} // namespace SP::ClientSettings
