#pragma once

#include <optional>
#include <variant>

#include <Common.hh>

class Archive {
public:
    struct File {
        u32 entrynum;
        const char *name;
        const u8 *data;
        u32 size;
    };

    struct Dir {
        u32 entrynum;
        const char *name;
        u32 parent;
        u32 next;
    };

    Archive(const u8 *data, u32 size);
    ~Archive() = default;
    bool ok() const;
    std::variant<std::monostate, File, Dir> get(u32 entrynum) const;
    std::variant<std::monostate, File, Dir> get(const char *path) const;

private:
    template <typename T>
    T read(u32 offset) const {
        T val = 0;
        _Pragma("GCC unroll(8)") for (size_t i = 0; i < sizeof(T); i++) {
            val |= static_cast<T>(m_data[offset + i]) << (8 * (sizeof(T) - i - 1));
        }
        return val;
    }

    std::optional<const char *> getString(u32 offset) const;

    const u8 *m_data;
    u32 m_size;
    u32 m_entriesOffset;
    u32 m_entryCount;
    u32 m_stringsOffset;
    u32 m_stringsSize;
    bool m_ok;
};
