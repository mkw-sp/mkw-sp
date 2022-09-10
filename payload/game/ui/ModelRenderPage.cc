#include "ModelRenderPage.hh"

#include "game/system/GameScene.h"
#include "game/system/ResourceManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/model/MenuModelManager.hh"

namespace UI {

void ModelRenderPage::onInit() {
    for (u8 i = 0; i < 4; i++) {
        _54[i]._00 = 0;
        _54[i]._04 = 4;
        _54[i]._08 = 0;
        _54[i]._09 = 0;
    }

    _88 = 0;
    _90 = 0;

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    auto sceneId = Section::GetSceneId(sectionId);
    if (sceneId != 1 && sceneId != 4) {
        _90 = 1;
        m_inputManager.init(0, false);
        setInputManager(&m_inputManager);
        initChildren(0);
        return;
    }

    // MenuModelManager nullptr check?
    m_modelCount = determineModelCount(sectionId);
    auto *gameScene = GameScene_get();
    HeapCollection_setGroupIdAll(&gameScene->volatileHeapCollection, 0);
    HeapCollection_setGroupIdAll(&gameScene->volatileHeapCollection, 3);

    if (sectionId < (SectionId)0x7f || sectionId > (SectionId)0x81) {
        System::ResourceManager::Instance()->createMenuHeaps(m_modelCount, 1);
    } else {
        System::ResourceManager::Instance()->createMenuHeaps(m_modelCount, 0);
    }

    MenuModelManager::Instance()->init(m_modelCount, &onDriverModelLoaded);
    HeapCollection_setGroupIdAll(&gameScene->volatileHeapCollection, 0);
    HeapCollection_setGroupIdAll(&gameScene->volatileHeapCollection, 6);
    
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

u8 ModelRenderPage::determineModelCount(SectionId sectionId) {
    if (sectionId == SectionId::VotingServer) { return 0; }
    return REPLACED(determineModelCount)(sectionId);
}

} // namespace UI
