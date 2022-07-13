#include "IOStream.hh"

namespace nw4r::ut {

IOStream::~IOStream() = default;

void IOStream::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~IOStream();
    }
}

} // namespace nw4r::ut
