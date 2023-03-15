#pragma once

#include "sp/storage/Storage.hh"

#include <array>

namespace SP {

class ThumbnailManager {
public:
    ThumbnailManager();
    ~ThumbnailManager();

    static bool Start();
    static bool Continue();
    static bool IsActive();
    static u32 CourseId();
    static std::array<wchar_t, 256> Path();

private:
    ThumbnailManager(const ThumbnailManager &) = delete;
    ThumbnailManager(ThumbnailManager &&) = delete;

    void nextDir();
    void nextName();
    void capture();
    std::array<wchar_t, 256> path();

    static bool Next();

    u32 m_courseId = 0;
    std::optional<Storage::DirHandle> m_dir{};
    std::optional<std::array<char, 128>> m_name{};

    static std::optional<ThumbnailManager> s_instance;
};

} // namespace SP
