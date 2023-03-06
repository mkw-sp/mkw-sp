#pragma once

#include "game/system/GhostFile.hh"
#include "game/ui/Page.hh"

namespace UI {

class GhostManagerPage : public Page {
public:
    class List {
    public:
        REPLACE void populate(u32 courseId);

    private:
        u8 _000[0x268 - 0x000];
    };
    static_assert(sizeof(List) == 0x268);

    class SPList {
    public:
        void populate();
        u16 count() const;
        const u16 *indices() const;

    private:
        u16 m_count;
        u16 m_indices[System::MAX_GHOST_COUNT];
    };

    GhostManagerPage();

    void REPLACED(process)();
    REPLACE void process();
    bool requestGhostReplay();
    bool requestGhostRace(bool isNewRecord, bool fromReplay);
    void requestPopulate();
    void repopulate();
    const SPList *list() const;

private:
    enum class Request {
        None = 0,
        SavedGhostRace = 3,
        SavedGhostReplay = 6,
        Populate = 14, // Added
    };

    void processPopulate();
    REPLACE void setupGhostReplay(bool isStaffGhost);
    REPLACE void setupGhostRace(bool isStaffGhost, bool isNewRecord, bool fromReplay);

    static void setupTimeAttack(bool isRace, bool fromReplay);

    u8 _0044[0x0054 - 0x0044];
    System::GhostGroup *m_groups[8];
    u8 _0074[0x0094 - 0x0074];
    Request m_currentRequest;
    u8 _0098[0x009c - 0x0098];
    Request m_nextRequest;
    u8 _00a0[0x2b0c - 0x00a0];
    SPList m_list; // Added
};

} // namespace UI
