#pragma once

#include "sp/storage/Storage.hh"

#include <game/util/Registry.hh>

#include <sp/FixedString.hh>

namespace SP {

class ThumbnailManager {
public:
    ThumbnailManager();
    ~ThumbnailManager();

    static bool Start();
    static bool Continue();
    static bool IsActive();
    static Registry::Course CourseId();
    static FixedString<64> Path();

private:
    ThumbnailManager(const ThumbnailManager &) = delete;
    ThumbnailManager(ThumbnailManager &&) = delete;

    void nextDir();
    void nextName();
    void capture();
    FixedString<64> path();

    static bool Next();

    u32 m_courseId = 0;
    std::optional<Storage::DirHandle> m_dir{};
    std::optional<std::array<char, 32>> m_name{};

    static std::optional<ThumbnailManager> s_instance;
};

} // namespace SP
