#include "DriverManager.hh"

namespace Race {

DriverManager *DriverManager::Instance() {
    return s_instance;
}

} // namespace Race
