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

    bool m_hasStarted;

    static std::optional<ThumbnailManager> s_instance;
};

} // namespace SP
