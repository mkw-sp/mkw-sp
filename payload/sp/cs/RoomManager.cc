#include "RoomManager.hh"

#include <egg/core/eggHeap.hh>
extern "C" {
#include <game/system/RootScene.h>
}
#include <game/ui/SectionManager.hh>

#include "sp/cs/RoomClient.hh"

namespace SP {

u32 RoomManager::playerCount() const {
    return m_playerCount;
}

const RoomManager::Player& RoomManager::player(u32 playerId) const {
    return m_players[playerId];
}

u32 RoomManager::gamemode() const {
    return m_gamemode;
}

void RoomManager::OnCreateScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    size_t size;
    if (UI::Section::HasRoomClient(sectionManager->nextSectionId())) {
        // Client
        if (UI::Section::HasRoomClient(sectionManager->lastSectionId())) {
            return;
        }

        size = sizeof(RoomClient);
    } else {
        return;
    }

    assert(!s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    s_block = heap->alloc(size, 0x4);
}

void RoomManager::OnDestroyScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    if (UI::Section::HasRoomClient(sectionManager->lastSectionId())) {
        if (UI::Section::HasRoomClient(sectionManager->nextSectionId())) {
            return;
        }

        if (RoomClient::Instance()) {
            RoomClient::DestroyInstance();
        }
    } else {
        return;
    }

    assert(s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    heap->free(s_block);
    s_block = nullptr;
}

RoomManager *RoomManager::Instance() {
    return s_instance;
}

RoomManager::RoomManager() {
    m_votePlayerOrder.fill(-1);
}

RoomManager::~RoomManager() = default;

void *RoomManager::s_block = nullptr;
RoomManager *RoomManager::s_instance = nullptr;

void RoomManager::Handler::onError(u32 errorCode) {
    auto *sectionManager = UI::SectionManager::Instance();
    sectionManager->transitionToError(errorCode);
}

} // namespace SP
