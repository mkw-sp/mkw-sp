#pragma once

#include "game/system/CourseMap.hh"
#include "game/system/InputManager.hh"
#include "game/system/RaceConfig.hh"

namespace System {

class RaceManager {
public:
    class Player {
    public:
        u8 rank() const;
        u8 maxLap() const;
        bool hasFinished() const;
        PadProxy *padProxy();

    private:
        u8 _00[0x20 - 0x00];
        u8 m_rank;
        u8 _21[0x26 - 0x21];
        u8 m_maxLap;
        u8 _27[0x38 - 0x27];
        u32 _pad0 : 30;
        bool m_hasFinished : 1;
        u32 _pad1 : 1;
        u8 _3c[0x48 - 0x3c];
        PadProxy *m_padProxy;
        u8 _4c[0x54 - 0x4c];
    };
    static_assert(sizeof(Player) == 0x54);

    Player *player(u32 playerId);
    void REPLACED(calc)();
    REPLACE void calc();

    void setSpectatorMode(bool toggle);
    bool spectatorMode() const;

    static RaceManager *Instance();
    static u8 GetLapCount();

private:
    class Mode {
    public:
        Mode(RaceManager *manager);
        ~Mode();
        virtual void vf_00();
        virtual void vf_04() {}
        virtual bool canRaceEnd();
        virtual void endLocalRace();
        virtual void calc();
        virtual void calcPosition();
        virtual MapdataJugemPoint *getJugemPoint(u8 playerIdx);
        virtual void init();
        virtual void vf_20();
        virtual bool tryForceEndRace(u32 param_1, u32 param_2);
        virtual void vf_28();

    protected:
        RaceManager *m_manager;
    };

    class OnlineClient : public Mode {
    public:
        OnlineClient(RaceManager *manager);
        ~OnlineClient();
        /*bool canRaceEnd() override;
        void endLocalRace() override;*/
        void calc() override;
        /*void calcPosition() override;
        void init() override;
        bool tryForceEndRace(u32 param_1, u32 param_2) override;*/
    };

    class OnlineServer : public Mode {
    public:
        OnlineServer(RaceManager *manager);
        ~OnlineServer();
        /*bool canRaceEnd() override;
        void endLocalRace() override;*/
        void calc() override;
        /*void calcPosition() override;
        void init() override;
        bool tryForceEndRace(u32 param_1, u32 param_2) override;*/
    };

    enum Stage {
        PreCountdown = 0,
        Countdown = 1,
        Race = 2,
        PostRace = 4,
    };

    Mode *REPLACED(initMode)(RaceConfig::GameMode mode);
    REPLACE Mode *initMode(RaceConfig::GameMode mode);

    u8 _00[0x0c - 0x00];
    Player **m_players;
    u8 _10[0x20 - 0x10];
    u32 m_frame;
    u8 _24[0x28 - 0x24];
    Stage m_stage;
    u8 _2c[0x2d - 0x2c];
    bool m_spectatorMode;
    bool m_startCountdown;
    u8 _2f[0x50 - 0x2f];

    static RaceManager *s_instance;
};
static_assert(sizeof(RaceManager) == 0x50);

} // namespace System
