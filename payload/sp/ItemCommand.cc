#include "ItemCommand.hh"
extern "C" {
#include "ItemCommand.h"

#include "sp/Commands.h"

#include <game/item/ItemDirector.h>
}
#include <game/system/RaceConfig.hh>

extern "C" {
#include <revolution.h>
}

namespace SP {

s8 GetMyPlayerID(void) {
    auto *raceConfig = System::RaceConfig::Instance();
    if (raceConfig == nullptr) {
        OSReport("&cNot in a race.\n");
        return -1;
    }

    auto &raceScenario = raceConfig->raceScenario();
    s8 playerId = raceScenario.screenPlayerIds[0];
    if (playerId < 0 || playerId > 11) {
        OSReport("&cCurrent player id is &4%i&c. Expected in range &4[0, 11]&c.", playerId);
        OSReport("Are you a spectator?\n");
        return -1;
    }

    return playerId;
}

sp_define_command("/i", "Spawn an item.", const char *args) {
    SP::ItemCommand::Invoke(args);
}

void ItemCommand::TrySet(s8 playerId) {
    if (s_sticky.has_value()) {
        TrySet(playerId, s_sticky->item, s_sticky->quantity);
    }
}

void ItemCommand::TrySet(s8 playerId, u8 item, u8 quantity) {
    if (s_itemDirector != nullptr) {
        s_itemDirector->m_kartItems[playerId].mCurrentItemKind = item;
        s_itemDirector->m_kartItems[playerId].mCurrentItemQty = quantity;
    }
}

void ItemCommand::ClearItem(s8 playerId) {
    TrySet(playerId, 20, 0);
    s_sticky.reset();
}

void ItemCommand::BeginFrame() {
    if (s_sticky.has_value()) {
        auto myPlayerId = GetMyPlayerID();
        if (myPlayerId >= 0) {
            TrySet(myPlayerId);
        }
    }
}

void ItemCommand::Invoke(const char *args) {
    auto myPlayerId = GetMyPlayerID();
    if (myPlayerId < 0) {
        return;
    }

    // No args
    if (!strcmp(args, "/i")) {
        OSReport("&aClearing item\n");
        ClearItem(myPlayerId);
        return;
    }

    // Args
    if (!strncmp(args, "/i ", strlen("/i "))) {
        u32 item = 0;
        u32 qty = 1;
        if (!sscanf(args, "/i %d %d", &item, &qty)) {
            OSReport("&aUnknown arguments \"%s\". Usage: /i [item] [qty|0=STICKY]\n",
                    args + strlen("/i "));
            return;
        }
        if (qty <= 0) {
            auto &stickyState = s_sticky.emplace();

            qty = MAX(-qty, 1);
            stickyState.item = item;
            stickyState.quantity = qty;
            OSReport("&aSpawning item %d (&2STICKY %dx&a)\n", item, qty);
        } else {
            ClearItem(myPlayerId);
            OSReport("&aSpawning item %d (&2%dx&a)\n", item, qty);
        }

        TrySet(myPlayerId, item, qty);
        return;
    }

    OSReport("&4Invalid command: \"%s\"", args);
}

std::optional<ItemCommand::StickyState> ItemCommand::s_sticky = std::nullopt;

} // namespace SP

extern "C" void Item_beginFrame() {
    SP::ItemCommand::BeginFrame();
}
