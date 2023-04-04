#pragma once

#include "sp/storage/Storage.hh"

#include <array>

namespace SP {

class ThumbnailManager {
public:
    ThumbnailManager() = default;
    ~ThumbnailManager() = default;

    static void Start();
    static bool Continue();
    static bool IsActive();
    static bool Next();

private:
    ThumbnailManager(const ThumbnailManager &) = delete;
    ThumbnailManager(ThumbnailManager &&) = delete;

    bool next();
    void capture();

    u32 m_trackIndex = 0;
    static std::optional<ThumbnailManager> s_instance;
};

} // namespace SP
