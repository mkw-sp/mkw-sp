#pragma once

#include "game/host_system/Scene.hh"

namespace System {

class GameScene : public Scene {
public:
    GameScene(const char *name);
    ~GameScene() override;
    void REPLACED(calc)();
    REPLACE void calc() override;
    void REPLACED(draw)();
    REPLACE void draw() override;
    void enter() override;
    void exit() override;
    void reinit() override;
    virtual void vf_28();
    virtual void vf_2c();
    virtual void calcSubsystems() = 0;
    virtual void destroySubsystems() = 0;
    virtual void vf_38() = 0;
    virtual void vf_3c() = 0;
    virtual void vf_40();
    virtual void createSubsystems() = 0;
    virtual void vf_48();
    virtual void vf_4c();
    virtual void vf_50();
    virtual void vf_54();
    virtual void vf_58();
    virtual void vf_5c();

    static GameScene *Instance();

private:
    void REPLACED(setFramerate)(bool is30FPS);
    REPLACE void setFramerate(bool is30FPS);

    u8 _0c70[0x0c94 - 0x0c70];

public:
    HeapCollection volatileHeapCollection;

private:
    u8 _18a0[0x2534 - 0x18a0];

protected:
    u32 m_time;

private:
    u8 _2538[0x2540 - 0x2538];

protected:
    bool m_isPaused;

private:
    u8 _2541[0x254c - 0x2541];
};
static_assert(sizeof(GameScene) == 0x254c);

} // namespace System
