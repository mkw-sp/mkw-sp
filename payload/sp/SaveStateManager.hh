#pragma once

#include "game/kart/KartSaveState.hh"

namespace SP {

class SaveStateManager {
public:
    void save();
    void reload();
    void processInput(bool isPressed);

    static void CreateInstance();
    static void DestroyInstance();
    static SaveStateManager *Instance() {
        return s_instance;
    };

private:
    static auto GetKartState();

    u8 m_framesHeld = 0;
    std::optional<Kart::KartSaveState> m_kartSaveState = std::nullopt;

    static SaveStateManager *s_instance;
};

} // namespace SP
