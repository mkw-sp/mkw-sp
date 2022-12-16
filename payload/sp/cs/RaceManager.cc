#include "RaceManager.hh"

extern "C" {
#include <game/system/RootScene.h>
}
#include <game/ui/SectionManager.hh>

#include "sp/cs/RaceClient.hh"
#include "sp/cs/RaceServer.hh"

namespace SP {

void RaceManager::OnCreateScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    size_t size;
    if (UI::Section::HasRaceClient(sectionManager->nextSectionId())) {
        // Client
        if (UI::Section::HasRaceClient(sectionManager->lastSectionId())) {
            return;
        }
        size = sizeof(RaceClient);
    } else if (UI::Section::HasRaceServer(sectionManager->nextSectionId())) {
        // Server
        if (UI::Section::HasRaceServer(sectionManager->lastSectionId())) {
            return;
        }
        size = sizeof(RaceServer);
    } else {
        return;
    }

    assert(!s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    s_block = heap->alloc(size, 0x4);
}

void RaceManager::OnDestroyScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    if (UI::Section::HasRaceClient(sectionManager->lastSectionId())) {
        // Client
        if (UI::Section::HasRaceClient(sectionManager->nextSectionId())) {
            return;
        }

        if (RaceClient::Instance()) {
            RaceClient::DestroyInstance();
        }
    } else if (UI::Section::HasRaceServer(sectionManager->lastSectionId())) {
        // Server
        if (UI::Section::HasRaceServer(sectionManager->nextSectionId())) {
            return;
        }

        if (RaceServer::Instance()) {
            RaceServer::DestroyInstance();
        }
    } else {
        return;
    }

    assert(s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    heap->free(s_block);
    s_block = nullptr;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

RaceManager::RaceManager() : m_roomManager(*RoomManager::Instance()) {}

RaceManager::~RaceManager() = default;

void *RaceManager::s_block = nullptr;
RaceManager *RaceManager::s_instance = nullptr;

} // namespace SP
