#include "CameraManager.hh"

namespace Graphics {

CameraManager *CameraManager::Instance() {
    return s_instance;
}

} // namespace Graphics
