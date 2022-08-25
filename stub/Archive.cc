#include "Archive.hh"

#include <cstring>

struct FSTEntry {
    u8 isDir : 8;
    u32 stringOffset : 24;
    union {
        struct {
            u32 _4;
            u32 next;
        } dir;
        struct {
            u32 startAddr;
            u32 length;
        } file;
    };
};
static_assert(sizeof(FSTEntry) == 0xc);

Archive::Archive(const u8 *data, u32 size) : m_data(data), m_size(size) {
    m_ok = false;

    if (m_size < 0x20 + 0xc) {
        return;
    }

    if (read<u32>(0x0) != 0x55AA382D) {
        return;
    }

    u32 metadataSize = read<u32>(0x8);
    if (metadataSize > 0x20 + m_size) {
        return;
    }

    m_entriesOffset = read<u32>(0x4);
    if (m_entriesOffset > m_size || m_entriesOffset + 0xc > m_size) {
        return;
    }
    m_entryCount = read<u32>(m_entriesOffset + 0x8);
    if (m_entriesOffset + m_entryCount * 0xc > m_size || m_entryCount * 0xc > metadataSize) {
        return;
    }

    m_stringsOffset = m_entriesOffset + m_entryCount * 0xc;
    m_stringsSize = metadataSize - m_entryCount * 0xc;
    if (m_stringsOffset > m_size || m_stringsOffset + m_stringsSize > m_size) {
        return;
    }

    m_ok = true;
}

bool Archive::ok() const {
    return m_ok;
}

std::variant<std::monostate, Archive::File, Archive::Dir> Archive::get(u32 entrynum) const {
    if (!ok()) {
        return std::monostate{};
    }

    if (entrynum >= m_entryCount) {
        return std::monostate{};
    }

    u32 offset = m_entriesOffset + entrynum * 0xc;
    u32 nameOffset = read<u32>(offset) & 0xffffff;
    std::optional<const char *> name = getString(m_stringsOffset + nameOffset);
    if (!name) {
        return std::monostate{};
    }

    u8 isDir = read<u8>(offset);
    if (isDir) {
        u32 parent = read<u32>(offset + 0x4);
        u32 next = read<u32>(offset + 0x8);
        if (parent >= m_entryCount || next > m_entryCount) {
            return std::monostate{};
        }
        if (next <= entrynum) {
            return std::monostate{};
        }
        return Dir{ entrynum, *name, parent, next };
    } else {
        const u8 *data = m_data + read<u32>(offset + 0x4);
        u32 size = read<u32>(offset + 0x8);
        u32 startOffset = data - m_data;
        u32 endOffset = startOffset + size;
        if (startOffset > m_size || endOffset > m_size) {
            return std::monostate{};
        }
        return File{ entrynum, *name, data, size };
    }
}

std::variant<std::monostate, Archive::File, Archive::Dir> Archive::get(const char *path) const {
    u32 entrynum = 0;
    auto entry = get(entrynum);

    while (*path != '\0' && std::holds_alternative<Dir>(entry)) {
        const char *sep = strchr(path, '/');
        u32 length = sep ? sep - path : strlen(path);

        Dir dir = std::get<Dir>(entry);
        for (entrynum = dir.entrynum + 1; entrynum < dir.next;) {
            entry = get(entrynum);
            std::optional<const char *> name{};
            if (auto *file = std::get_if<File>(&entry)) {
                name = file->name;
            } else if (auto *dir = std::get_if<Dir>(&entry)) {
                name = dir->name;
            }
            if (!name) {
                return std::monostate{};
            }
            if (!strncmp(*name, path, length)) {
                break;
            }
            Dir *subdir = std::get_if<Dir>(&entry);
            if (subdir) {
                entrynum = subdir->next;
            } else {
                entrynum++;
            }
        }
        if (entrynum == dir.next) {
            return std::monostate{};
        }

        path = sep ? path + length + 1 : path + length;
    }

    if (*path != '\0') {
        return std::monostate{};
    }
    return entry;
}

std::optional<const char *> Archive::getString(u32 offset) const {
    const char *string = reinterpret_cast<const char *>(m_data + offset);
    for (; offset < m_size; offset++) {
        if (read<char>(offset) == '\0') {
            return string;
        }
    }

    return {};
}
