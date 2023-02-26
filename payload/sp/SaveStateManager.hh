#pragma once

#include "game/kart/KartSaveState.hh"

namespace SP {

class SaveStateManager {
public:
    SaveStateManager();
    ~SaveStateManager();

    void save();
    void reload();
    void processInput(bool isPressed);

    static void CreateInstance();
    static void DestroyInstance();
    static SaveStateManager* Instance() {return s_instance;};
private:
    static auto GetKartState();

    u8 m_framesHeld;
    bool m_hasSaved;

    Kart::KartSaveState *m_kartSaveState;
    static SaveStateManager* s_instance;
};

} // namespace System
