#include "U8Iterator.hh"

#include "sp/storage/Storage.hh"

namespace SP {

void U8Iterator::reset(size_t startPos) {
    m_iteration = 0;
    m_dirStack.reset();
    m_cursor.setPosition(startPos);
}

std::array<wchar_t, 64> U8Iterator::getPath(const char *name) const {
    std::array<wchar_t, 64> path;
    std::array<wchar_t, 64> pathBuf;
    wcscpy(pathBuf.data(), L"autoadd");

    for (u8 i = 0; i < m_dirStack.count(); i++) {
        auto dirName = m_cursor.readString(m_stringTableStart, m_dirStack[i]->nameOffset);

        // Avoid writing to path while it is being read by double buffering.
        swprintf(path.data(), path.size(), L"%ls/%s", pathBuf.data(), dirName);
        pathBuf = path;
    }

    if (name == nullptr) {
        return pathBuf;
    } else {
        swprintf(path.data(), path.size(), L"%ls/%s", pathBuf.data(), name);
        return path;
    }
}

std::optional<U8IterItem> U8Iterator::next(bool skipReadingFile) {
    if (m_iteration == m_nodeCount) {
        return std::nullopt;
    }

    m_iteration++;

    auto node = m_cursor.readNode();
    if (!node.has_value()) {
        return U8IterItem::Err();
    }

    if (node->nameOffset == 0 || node->nameOffset == 1) {
        return next(skipReadingFile);
    }

    if (m_dirStack.count() != 0) {
        for (s8 i = m_dirStack.count() - 1; i != -1; i--) {
            auto *currentDir = m_dirStack.back();
            if (currentDir->size == m_iteration - 1) {
                m_dirStack.remove(m_dirStack.count() - 1);
            } else {
                break;
            }
        }
    }

    auto name = m_cursor.readString(m_stringTableStart, node->nameOffset);
    if (node->isDir) {
        m_dirStack.push_back(std::move(*node));
        return U8IterItem::Dir();
    }

    auto path = getPath(name);
    auto file = Storage::Open(path.data(), "r");
    bool hasData = false;
    if (file.has_value()) {
        hasData = true;
        if (!skipReadingFile) {
            assert(m_fileOut != nullptr);
            m_fileOut->resize(file->size());
            if (!file->read(m_fileOut->data(), file->size(), 0)) {
                return U8IterItem::Err();
            }
        }
    }

    U8IterFile outFile;
    outFile.name = name;
    outFile.node = *node;
    outFile.hasData = hasData;
    return U8IterItem::File(outFile);
}

} // namespace SP
