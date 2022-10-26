#include "RoomManager.hh"

#include <egg/core/eggHeap.hh>
extern "C" {
#include <game/system/RootScene.h>
}
#include <game/ui/SectionManager.hh>

#include "sp/cs/RoomClient.hh"
#include "sp/cs/RoomServer.hh"

namespace SP {

RoomManager::RoomManager() = default;

RoomManager::~RoomManager() = default;

void RoomManager::OnCreateScene() {
    size_t size;
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    if (UI::Section::HasRoomClient(sectionManager->nextSectionId())) {
        // Client
        if (UI::Section::HasRoomClient(sectionManager->lastSectionId())) {
            return;
        }
        size = sizeof(RoomClient);
    } else if (UI::Section::HasRoomServer(sectionManager->nextSectionId())) {
        // Server
        if (UI::Section::HasRoomServer(sectionManager->lastSectionId())) {
            return;
        }
        size = sizeof(RoomServer);
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
        // Client
        if (UI::Section::HasRoomClient(sectionManager->nextSectionId())) {
            return;
        }

        if (RoomClient::Instance()) {
            RoomClient::DestroyInstance();
        }
    } else if (UI::Section::HasRoomServer(sectionManager->lastSectionId())) {
        // Server
        if (UI::Section::HasRoomServer(sectionManager->nextSectionId())) {
            return;
        }

        if (RoomServer::Instance()) {
            RoomServer::DestroyInstance();
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

void *RoomManager::s_block = nullptr;
RoomManager *RoomManager::s_instance = nullptr;

} // namespace SP
