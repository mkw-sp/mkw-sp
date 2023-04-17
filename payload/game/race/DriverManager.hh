#pragma once

#include <Common.hh>

namespace Race {

class DriverManager {
public:
    DriverManager();
    virtual ~DriverManager();
    virtual void dt(s32 type);

    void calc();

    REPLACE static DriverManager *CreateInstance();
    static DriverManager *Instance();

private:
    enum class Outcome {
        Best,
        Good,
        Bad,
        Draw,
    };

    REPLACE static Outcome GetTeamPlayerOutcome(u32 playerId);

    u8 _004[0x1ec - 0x004];
    u8 m_liveViewWatchedPlayerId;
    u8 _1ed[0x1f0 - 0x1ed];

    static DriverManager *s_instance;
};
static_assert(sizeof(DriverManager) == 0x1f0);

} // namespace Race
