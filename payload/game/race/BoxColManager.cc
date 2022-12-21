#include "BoxColManager.hh"

namespace Race {

BoxColManager *BoxColManager::Instance() {
    return s_instance;
}

} // namespace Race
