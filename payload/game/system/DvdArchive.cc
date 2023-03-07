#include "DvdArchive.hh"

extern "C" {
#include <revolution.h>
}

#include <sp/ThumbnailManager.hh>
#include <sp/cs/RoomManager.hh>
#include <sp/settings/ClientSettings.hh>
#include <sp/storage/DecompLoader.hh>

#include "game/system/SaveManager.hh"

#include <cstring>

namespace System {

void DvdArchive::load(const char *path, EGG::Heap *archiveHeap, bool isCompressed, s8 align,
        EGG::Heap *fileHeap, u32) {
    if (m_state != State::Cleared) {
        return;
    }

    if (isCompressed) {
        return decompress(path, archiveHeap) ? mount(archiveHeap) : (void)0;
    } else if (!fileHeap) {
        fileHeap = archiveHeap;
    }

    rip(path, fileHeap, align);
    if (m_state == State::Ripped) {
        move();
        mount(archiveHeap);
    }
}

void DvdArchive::loadOther(DvdArchive *other, EGG::Heap *) {
    if (m_state != State::Cleared || other->m_state != State::Mounted) {
        return;
    }

    m_archiveBuffer = other->m_archiveBuffer;
    m_archiveSize = other->m_archiveSize;
    m_archiveHeap = other->m_archiveHeap;
    mount(m_archiveHeap);
    other->m_archiveBuffer = nullptr;
    other->m_archiveSize = 0;
    other->m_archiveHeap = nullptr;
    other->clear();
}

void *DvdArchive::getFile(const char *path, size_t *size) {
    if (SP::ThumbnailManager::IsActive()) {
        if (!strcmp(path, "race_camera.bcp") || !strcmp(path, "start_camera.bcp")) {
            return nullptr;
        }

        if (!strcmp(path, "kartCameraParam.bin")) {
            return REPLACED(getFile)("kartCameraParamThumbnails.bin", size);
        }
    }

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::VSMegaClouds>();
    if (setting == SP::ClientSettings::VSMegaClouds::Enable) {
        if (!strcmp(path, "kumo.brres")) {
            return REPLACED(getFile)("MegaTC.brres", size);
        }
    }

    if (SP::RoomManager::Instance()) {
        if (!strcmp(path, "ItemSlot.bin")) {
            return REPLACED(getFile)("ItemSlotOnline.bin", size);
        }
    }

    return REPLACED(getFile)(path, size);
}

void *DvdArchive::buffer() const {
    return m_archiveBuffer;
}

size_t DvdArchive::size() const {
    return m_archiveSize;
}

DvdArchive::State DvdArchive::state() const {
    return m_state;
}

bool DvdArchive::decompress(const char *path, EGG::Heap *archiveHeap) {
    u8 *archiveBuffer;
    size_t archiveSize;
    bool success = SP::Storage::DecompLoader::LoadRO(path, &archiveBuffer, &archiveSize, archiveHeap);
    if (success) {
        m_archiveBuffer = archiveBuffer;
        m_archiveSize = archiveSize;
        m_archiveHeap = archiveHeap;
        m_state = State::Decompressed;
    }

    return success;
}

} // namespace System
