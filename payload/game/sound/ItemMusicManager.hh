#pragma once

#include <Common.hh>

namespace Sound {

class ItemMusicManager {
public:
    static ItemMusicManager *REPLACED(CreateInstance)();
    REPLACE static ItemMusicManager *CreateInstance();
    static ItemMusicManager *Instance();

private:
    enum class State {
        KinokoBig = 0,
        Star = 1,
        Damage = 2,
        Idle = 3,
    };

    void REPLACED(resolve)();
    REPLACE void resolve();
    f32 pitch() const;

    u8 _00[0x10 - 0x00];
    State m_state;
    u8 _14[0x17 - 0x14];

public:
    bool m_speedup; // Added (was padding)

private:
    u8 _18[0x20 - 0x18];
    f32 m_volume;
    f32 m_pitch;
    u8 _28[0x2c - 0x28];

    static ItemMusicManager *s_instance;
};
static_assert(sizeof(ItemMusicManager) == 0x2c);

} // namespace Sound
