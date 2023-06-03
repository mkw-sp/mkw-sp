#pragma once

#include <Common.hh>
#include <optional>

namespace SP {

class ItemCommand {
public:
    static void BeginFrame();
    static void Invoke(const char *args);

private:
    struct StickyState {
        u8 item;
        u8 quantity;
    };

    static void TrySet(s8 playerId);
    static void TrySet(s8 playerId, u8 item, u8 quantity);

    static void ClearItem(s8 playerId);

    static std::optional<StickyState> s_sticky;
};

} // namespace SP
