#include "lyt_texMap.hh"

namespace nw4r::lyt {

TexMap::TexMap(const GXTexObj &texObj) {
    set(texObj);

    _04 = 0;
    _18 &= 0xffff9fff;
    _16 = 0;
}

} // namespace nw4r::lyt
