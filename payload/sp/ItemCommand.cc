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

static s8 GetMyPlayerID() {
    auto &raceScenario = System::RaceConfig::Instance()->raceScenario();
    s8 playerId = raceScenario.screenPlayerIds[0];

    if (playerId < 0 || playerId > 11) {
        OSReport("&cCurrent player id is &4%i&c. Expected in range &4[0, 11]&c.\n", playerId);
        return -1;
    } else if (raceScenario.players[playerId].type != System::RaceConfig::Player::Type::Local) {
        OSReport("&cCurrent player id is &4%i&c, but is not a local player.\n", playerId);
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
    if (strncmp(args, "/i ", strlen("/i "))) {
        OSReport("&4Invalid command: \"%s\"", args);
        return;
    }

    u8 item = 0;
    u8 quantity = 1;
    if (!sscanf(args, "/i %hhu %hhu", &item, &quantity)) {
        OSReport("&aUnknown arguments \"%s\". Usage: /i [item] [qty|0=STICKY]\n",
                args + strlen("/i "));
        return;
    }

    if (quantity <= 0) {
        quantity = std::max(-quantity, 1);

        s_sticky.emplace() = {.item = item, .quantity = quantity};
        OSReport("&aSpawning item %d (&2STICKY %dx&a)\n", item, quantity);
    } else {
        ClearItem(myPlayerId);
        OSReport("&aSpawning item %d (&2%dx&a)\n", item, quantity);
    }

    TrySet(myPlayerId, item, quantity);
}

std::optional<ItemCommand::StickyState> ItemCommand::s_sticky = std::nullopt;

} // namespace SP

extern "C" void Item_beginFrame() {
    SP::ItemCommand::BeginFrame();
}
