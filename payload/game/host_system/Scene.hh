#pragma once

#include <egg/core/eggExpHeap.hh>
#include <egg/core/eggHeap.hh>
#include <egg/core/eggScene.hh>

namespace System {

class HeapCollection {
public:
    void setGroupIdAll(u32 groupId);

    EGG::Heap *mem1;
    EGG::Heap *mem2;
    EGG::Heap *debug;
    std::array<EGG::ExpHeap::GroupSizeRecord, 3> groupSizeRecords;
};

static_assert(sizeof(HeapCollection) == 0xc0c);

class Scene : public EGG::Scene {
public:
    Scene();
    ~Scene() override;

    HeapCollection m_heapCollection;

private:
    u8 _c3c[0xc70 - 0xc3c];
};

static_assert(sizeof(Scene) == 0xc70);

enum class SceneId {
    // --- <SceneCreatorREL>
    Title = 0, //!< Called when REL first loaded
    Menu = 1,  //!< "メニューシーン" -> "Menu Scene"
    Race = 2,  //!< "レースシーン" -> "Race Scene"
    Globe = 4, //!< "地球儀シーン" -> "Globe Scene"

    // --- <SceneCreatorStatic>
    Strap = 5, //!< Root scene loaded immediately after initializing the system.
    // --- </SceneCreatorStatic>

    FlagOpen = 12, //!< "フラグオープンシーン" -> "Flag Open Scene"
    // --- </SceneCreatorREL>
};

} // namespace System
