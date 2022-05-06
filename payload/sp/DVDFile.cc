#include "DVDFile.hh"

namespace SP {

DVDFile::DVDFile(const char *path) {
    m_ok = DVDOpen(path, &m_info);
}

DVDFile::~DVDFile() {
    if (m_ok) {
        DVDClose(&m_info);
    }
}

u32 DVDFile::size() const {
    assert(m_ok);
    return m_info.length;
}

u32 DVDFile::alignedSize() const {
    assert(m_ok);
    return OSRoundUp32B(m_info.length);
}

s32 DVDFile::read(void *dst, s32 size, s32 offset) {
    assert(m_ok);
    s32 result = DVDRead(&m_info, dst, size, offset);
    if (result < 0) {
        m_ok = false;
    }
    return result;
}

bool DVDFile::ok() const {
    return m_ok;
}

} // namespace SP
