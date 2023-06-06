#pragma once

#include "game/host_system/Scene.hh"
#include "game/system/MultiDvdArchive.hh"

namespace System {

class GameScene : public Scene {
public:
    GameScene(const char *name);
    ~GameScene() override;
    void calc() override;
    void draw() override;
    void enter() override;
    void exit() override;
    void reinit() override;
    virtual void vf_28();
    virtual void vf_2c();
    virtual void calcSubsystems() = 0;
    virtual void destroySubsystems() = 0;
    virtual void loadArchives() = 0;
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

protected:
    void registerArchive(MultiDvdArchive *arc, MultiDvdArchive::Type chan);

private:
    void REPLACED(setFramerate)(bool is_30);
    REPLACE void setFramerate(bool is_30);

    u32 _C70;
    nw4r::ut::List m_archiveList;
    EGG::ExpHeap *m_rootMem1;
    EGG::ExpHeap *m_rootMem2;
    EGG::ExpHeap *m_volatileMem1;
    EGG::ExpHeap *m_volatileMem2;
    EGG::ExpHeap *m_volatileDebug;

public:
    HeapCollection volatileHeapCollection;

private:
    u8 _18A0[0x18d4 - 0x18a0];

public:
    HeapCollection m_rootHeapCollection;

private:
    u8 _24e0[0x2514 - 0x24e0];
    u32 m_dummyAlloc0Size;
    u32 m_dummyAlloc1Size;
    void *m_dummyAlloc0;
    void *m_dummyAlloc1;
    u8 _2524[0x2530 - 0x2524];
    u8 _2530;
    u8 _2531;
    u8 _2532[0x2534 - 0x2532];

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
