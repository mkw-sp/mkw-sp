#include "ItemCommand.h"
#include "Commands.h"

#include <game/item/ItemDirector.h>
#include <game/system/RaceConfig.h>

#include <revolution.h>

bool sItemSticky = false;
int sStickyItem = 0;
int sStickyQty = 1;

s32 GetMyPlayerID(void) {
    if (s_raceConfig == NULL) {
        OSReport("&cNot in a race.\n");
        return -1;
    }
    s32 myPlayerId = s_raceConfig->raceScenario.localPlayerIds[0];
    if (myPlayerId < 0 || myPlayerId > 11) {
        OSReport(
                "&cCurrent player id is &4%i&c. Expected in range &4[0, 11]&c. Are you a "
                "spectator?\n",
                myPlayerId);
        return -1;
    }
    return myPlayerId;
}

void ClearItem(s32 myPlayerId) {
    if (s_itemDirector != NULL) {
        s_itemDirector->m_kartItems[myPlayerId].mCurrentItemKind = 20;
        s_itemDirector->m_kartItems[myPlayerId].mCurrentItemQty = 0;
    }
    sItemSticky = false;
}

void TrySetItem(s32 myPlayerId, s32 item, s32 qty) {
    if (s_itemDirector != NULL) {
        s_itemDirector->m_kartItems[myPlayerId].mCurrentItemKind = item;
        s_itemDirector->m_kartItems[myPlayerId].mCurrentItemQty = qty;
    }
}

void Item_beginFrame() {
    if (sItemSticky) {
        const s32 myPlayerId = GetMyPlayerID();
        if (myPlayerId >= 0) {
            TrySetItem(myPlayerId, sStickyItem, sStickyQty);
        }
    }
}

sp_define_command("/i", "Spawn an item.", const char *tmp) {
    const s32 myPlayerId = GetMyPlayerID();
    if (myPlayerId < 0) {
        return;
    }

    // No args
    if (!strcmp(tmp, "/i")) {
        OSReport("&aClearing item\n");
        ClearItem(myPlayerId);
        return;
    }
    // Args
    if (!strncmp(tmp, "/i ", strlen("/i "))) {
        int item = 0;
        int qty = 1;
        if (!sscanf(tmp, "/i %d %d", &item, &qty)) {
            OSReport("&aUnknown arguments \"%s\". Usage: /i [item] [qty|0=STICKY]\n",
                    tmp + strlen("/i "));
            return;
        }
        if (qty <= 0) {
            qty = MAX(-qty, 1);
            sItemSticky = true;
            sStickyItem = item;
            sStickyQty = qty;
            OSReport("&aSpawning item %d (&2STICKY %dx&a)\n", item, qty);
        } else {
            sItemSticky = false;
            OSReport("&aSpawning item %d (&2%dx&a)\n", item, qty);
        }

        TrySetItem(myPlayerId, item, qty);
        return;
    }

    OSReport("&4Invalid command: \"%s\"", tmp);
}
