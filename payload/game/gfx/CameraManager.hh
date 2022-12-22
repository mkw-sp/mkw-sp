#pragma once

#include <Common.hh>

namespace Graphics {

class CameraManager {
public:
    bool isReady() const;

    static CameraManager *Instance();

private:
    u8 _00[0x38 - 0x00];

    static CameraManager *s_instance;
};
static_assert(sizeof(CameraManager) == 0x38);

} // namespace Graphics
