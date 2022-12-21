#include "ItemManager.hh"

namespace Item {

ItemManager *ItemManager::Instance() {
    return s_instance;
}

} // namespace Item
