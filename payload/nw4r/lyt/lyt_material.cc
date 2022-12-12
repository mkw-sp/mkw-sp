#include "lyt_material.hh"

namespace nw4r::lyt {

u8 Material::getTextureNum() const {
    return _3c >> 28 & 0xf;
}

u8 Material::getTexSRTNum() const {
    return _3c >> 24 & 0xf;
}

} // namespace nw4r::lyt
