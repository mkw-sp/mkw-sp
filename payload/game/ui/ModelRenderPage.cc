#include "ModelRenderPage.hh"

#include "game/system/GameScene.hh"
#include "game/system/ResourceManager.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/model/MenuModelManager.hh"

namespace UI {

void ModelRenderPage::onInit() {
    for (u8 i = 0; i < 4; i++) {
        m_players[i].m_characterId = Registry::Character::Mario;
        m_players[i].m_vehicleId = Registry::Vehicle::ClassicDragster;
        m_players[i]._08 = false;
        m_players[i]._09 = false;
    }

    _88 = 0;
    _90 = 0;

    if (!MenuModelManager::Instance()) {
        _90 = 1;
        m_inputManager.init(0, false);
        setInputManager(&m_inputManager);
        initChildren(0);
        return;
    }

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    m_modelCount = determineModelCount(sectionId);

    auto *gameScene = System::GameScene::Instance();
    gameScene->volatileHeapCollection.setGroupIdAll(3);

    System::ResourceManager::Instance()->createMenuHeaps(m_modelCount, 1);
    MenuModelManager::Instance()->init(m_modelCount, &onDriverModelLoaded);

    gameScene->volatileHeapCollection.setGroupIdAll(6);

    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);
    initChildren(0);
}

bool ModelRenderPage::player_09(u8 localPlayerId) const {
    return m_players[localPlayerId]._09;
}

} // namespace UI
