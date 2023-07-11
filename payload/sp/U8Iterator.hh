#pragma once

#include "U8Cursor.hh"

#include "sp/CircularBuffer.hh"
#include "sp/HeapAllocator.hh"

#include <vector>

namespace SP {

struct U8IterFile {
    U8Node node;
    bool hasData;
    const char *name;
};

// An enum I swear
class U8IterItem {
public:
    static U8IterItem File(U8IterFile file) {
        U8IterItem self;
        self.file = file;
        self.isDir = false;
        self.isErr = false;
        return self;
    }

    static U8IterItem Err() {
        U8IterItem self;
        self.file = std::nullopt;
        self.isDir = false;
        self.isErr = true;
        return self;
    }

    static U8IterItem Dir() {
        U8IterItem self;
        self.file = std::nullopt;
        self.isDir = true;
        self.isErr = false;
        return self;
    }

    std::optional<U8IterFile> file;
    bool isDir;
    bool isErr;
};

class U8Iterator {
public:
    U8Iterator(U8Cursor &cursor, std::vector<u8, HeapAllocator<u8>> *fileOut, u32 nodeCount,
            u32 stringTableStart)
        : m_nodeCount(nodeCount), m_stringTableStart(stringTableStart), m_cursor(cursor),
          m_fileOut(fileOut){};

    void reset(size_t startPos);
    // May be null, to just get the dir path.
    std::array<wchar_t, 64> getPath(const char *name) const;

    std::optional<U8IterItem> next(bool skipReadingFile);

private:
    u32 m_iteration = 0;

    u32 m_nodeCount;
    u32 m_stringTableStart;
    CircularBuffer<U8Node, 10> m_dirStack;

    U8Cursor &m_cursor;
    std::vector<u8, HeapAllocator<u8>> *m_fileOut;
};

} // namespace SP
