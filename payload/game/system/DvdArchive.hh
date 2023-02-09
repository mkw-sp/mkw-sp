#pragma once

#include <egg/core/eggHeap.hh>

namespace System {

class DvdArchive {
public:
    enum class State {
        Cleared = 0,
        Ripped = 2,
        Decompressed = 3,
        Mounted = 4,
    };

    virtual ~DvdArchive();

    void rip(const char *path, EGG::Heap *fileHeap, s8 align);

    REPLACE void load(const char *path, EGG::Heap *archiveHeap, bool isCompressed, s8 align,
            EGG::Heap *fileHeap, u32 UNUSED(unused));
    REPLACE void loadOther(DvdArchive *other, EGG::Heap *UNUSED(unused));

    void *REPLACED(getFile)(const char *path, size_t *size);
    REPLACE void *getFile(const char *path, size_t *size);

    void *buffer() const;
    size_t size() const;
    State state() const;

private:
    void clear();
    bool decompress(const char *path, EGG::Heap *archiveHeap);
    void mount(EGG::Heap *archiveHeap);
    void move();

    u8 _04[0x08 - 0x04];
    void *m_archiveBuffer;
    size_t m_archiveSize;
    EGG::Heap *m_archiveHeap;
    void *m_fileBuffer;
    size_t m_fileSize;
    EGG::Heap *m_fileHeap;
    State m_state;
};
static_assert(sizeof(DvdArchive) == 0x24);

} // namespace System
