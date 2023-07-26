#include "ThumbnailManager.hh"

#include <egg/core/eggSystem.hh>
#include <egg/core/eggXfbManager.hh>

#include <cwchar>
#include <iterator>

namespace SP {

bool ThumbnailManager::Start() {
    s_instance.emplace();

    return Next();
}

bool ThumbnailManager::Continue() {
    assert(s_instance);
    s_instance->capture();

    return Next();
}

bool ThumbnailManager::IsActive() {
    return s_instance.has_value();
}

Registry::Course ThumbnailManager::CourseId() {
    assert(s_instance);
    return static_cast<Registry::Course>(s_instance->m_courseId - 1);
}

FixedString<64> ThumbnailManager::Path() {
    assert(s_instance);
    return s_instance->path();
}

ThumbnailManager::ThumbnailManager() {
    nextDir();
}

ThumbnailManager::~ThumbnailManager() = default;

void ThumbnailManager::nextDir() {
    m_dir.reset();
    for (; !m_dir && m_courseId < 0x20; m_courseId++) {
        std::array<wchar_t, 256> path{};
        swprintf(path.data(), path.size(), L"/mkw-sp/thumbnails/inputs/%u", m_courseId);
        m_dir = Storage::OpenDir(path.data());
    }
}

void ThumbnailManager::nextName() {
    m_name.reset();
    while (m_dir) {
        while (auto info = m_dir->read()) {
            if (info->type != Storage::NodeType::File) {
                continue;
            }

            std::array<char, 32> name{};
            if (snprintf(name.data(), name.size(), "%ls", info->name) >=
                    static_cast<s32>(name.size())) {
                continue;
            }

            m_name = name;
            return;
        }
        nextDir();
    }
}

void ThumbnailManager::capture() {
    std::array<wchar_t, 256> path{};

    swprintf(path.data(), path.size(), L"/mkw-sp/thumbnails/outputs");
    if (!Storage::CreateDir(path.data(), true)) {
        return;
    }

    swprintf(path.data(), path.size(), L"/mkw-sp/thumbnails/outputs/%u", m_courseId - 1);
    if (!Storage::CreateDir(path.data(), true)) {
        return;
    }

    swprintf(path.data(), path.size(), L"/mkw-sp/thumbnails/outputs/%u/%s.xfb", m_courseId - 1,
            m_name->data());
    auto *xfb = EGG::TSystem::Instance().getXfbManager()->headXfb();
    u32 size = EGG::Xfb::CalcXfbSize(xfb->width(), xfb->height());
    Storage::WriteFile(path.data(), xfb->buffer(), size, true);
}

FixedString<64> ThumbnailManager::path() {
    FixedString<64> path{};
    auto len = snprintf(path.m_buf.data(), 64, "/mkw-sp/thumbnails/inputs/%u/%s", m_courseId - 1,
            m_name->data());
    assert(len > 0);

    path.m_len = len;
    return path;
}

bool ThumbnailManager::Next() {
    if (s_instance) {
        s_instance->nextName();
        if (!s_instance->m_name) {
            s_instance.reset();
        }
    }

    return IsActive();
}

std::optional<ThumbnailManager> ThumbnailManager::s_instance{};

} // namespace SP
