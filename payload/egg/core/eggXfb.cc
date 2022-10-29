#include "eggXfb.hh"

namespace EGG {

u16 Xfb::width() const {
    return m_width;
}

u16 Xfb::height() const {
    return m_height;
}

void *Xfb::buffer() {
    return m_buffer;
}

} // namespace EGG
