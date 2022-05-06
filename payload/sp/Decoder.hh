#pragma once

extern "C" {
#include <Common.h>
}

namespace SP {

class Decoder {
public:
    virtual ~Decoder() {}
    virtual bool decode(const u8 *src, size_t size) = 0;
    virtual void release(u8 **dst, size_t *dstSize) = 0;
    virtual bool ok() const = 0;
    virtual bool done() const = 0;
    virtual size_t headerSize() const = 0;
};

} // namespace SP
