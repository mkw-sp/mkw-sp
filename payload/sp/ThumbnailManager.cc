#include "ThumbnailManager.hh"

extern "C" {
#include "sp/WideUtil.h"
}
#include "sp/TrackPackManager.hh"

#include <egg/core/eggSystem.hh>
#include <game/system/RaceConfig.hh>

#include <cwchar>
#include <iterator>

namespace SP {

void ThumbnailManager::Start() {
    s_instance.emplace();
}

bool ThumbnailManager::Continue() {
    assert(s_instance);
    s_instance->capture();

    return Next();
}

bool ThumbnailManager::IsActive() {
    return s_instance.has_value();
}

bool ThumbnailManager::next() {
    auto &trackPackManager = TrackPackManager::Instance();
    auto &trackPack = trackPackManager.getSelectedTrackPack();
    auto trackId = trackPack.getNthTrack(m_trackIndex++, TrackGameMode::Race);

    if (trackId.has_value()) {
        auto *raceConfig = System::RaceConfig::Instance();
        raceConfig->m_packInfo.selectCourse(*trackId);
        return true;
    } else {
        return false;
    }
}

void ThumbnailManager::capture() {
    std::array<wchar_t, 256> path{};

    swprintf(path.data(), path.size(), L"/mkw-sp/Generated Thumbnails");
    if (!Storage::CreateDir(path.data(), true)) {
        return;
    }

    auto sha1 = System::RaceConfig::Instance()->m_packInfo.getSelectedSha1();
    auto hex = sha1ToHex(sha1);

    swprintf(path.data(), path.size(), L"/mkw-sp/Generated Thumbnails/%s.xfb", hex.data());

    auto *xfb = EGG::TSystem::Instance()->xfbManager()->headXfb();
    u32 size = EGG::Xfb::CalcXfbSize(xfb->width(), xfb->height());
    Storage::WriteFile(path.data(), xfb->buffer(), size, true);
}

bool ThumbnailManager::Next() {
    if (s_instance && !s_instance->next()) {
        s_instance.reset();
    }

    return IsActive();
}

std::optional<ThumbnailManager> ThumbnailManager::s_instance{};

} // namespace SP
