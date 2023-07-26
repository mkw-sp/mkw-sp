#include "GhostManagerPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <algorithm>
#include <cstring>

namespace UI {

void GhostManagerPage::List::populate(u32 /* courseId */) {}

void GhostManagerPage::SPList::populate() {
    auto *saveManager = System::SaveManager::Instance();
    auto *raceConfig = System::RaceConfig::Instance();

    auto courseId = raceConfig->menuScenario().courseId;
    auto cc = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    bool speedModIsEnabled = cc == SP::ClientSettings::TAClass::CC200;

    Sha1 courseSha1;
    if (raceConfig->m_spMenu.courseSha.has_value()) {
        courseSha1 = *raceConfig->m_spMenu.courseSha;
    } else {
        courseSha1 = saveManager->courseSHA1(courseId);
    }

    m_count = 0;
    for (u32 i = 0; i < saveManager->ghostCount(); i++) {
        auto *header = saveManager->rawGhostHeader(i);
        auto *footer = saveManager->ghostFooter(i);
        if (footer->courseSHA1() && footer->courseSHA1() != courseSha1) {
            continue;
        }
        if (footer->hasSpeedMod() && *(footer->hasSpeedMod()) != speedModIsEnabled) {
            continue;
        }
        if (header->courseId != courseId) {
            continue;
        }
        m_indices[m_count++] = i;
    }
    std::sort(std::begin(m_indices), std::begin(m_indices) + m_count, [&](auto i0, auto i1) {
        auto *h0 = saveManager->rawGhostHeader(i0);
        auto *h1 = saveManager->rawGhostHeader(i1);
        switch (saveManager->getSetting<SP::ClientSettings::Setting::TAGhostSorting>()) {
        case SP::ClientSettings::TAGhostSorting::Time:
            return h0->raceTime.toMilliseconds() < h1->raceTime.toMilliseconds();
        case SP::ClientSettings::TAGhostSorting::Date: {
            u32 d0 = (h0->year * 12 + h0->month) * 31 + h0->day;
            u32 d1 = (h1->year * 12 + h1->month) * 31 + h1->day;
            return d0 > d1;
        }
        case SP::ClientSettings::TAGhostSorting::Flap:
            return h0->flap()->toMilliseconds() < h1->flap()->toMilliseconds();
        case SP::ClientSettings::TAGhostSorting::Lap2Pace:
            if (h0->lapCount >= 2) {
                return h0->lapTimes[0].toMilliseconds() < h1->lapTimes[0].toMilliseconds();
            }
            break;
        case SP::ClientSettings::TAGhostSorting::Lap3Pace:
            if (h0->lapCount >= 3) {
                u32 l3p0 = h0->lapTimes[0].toMilliseconds() + h0->lapTimes[1].toMilliseconds();
                u32 l3p1 = h1->lapTimes[0].toMilliseconds() + h1->lapTimes[1].toMilliseconds();
                return l3p0 < l3p1;
            }
            break;
        }
        return h0->raceTime.toMilliseconds() < h1->raceTime.toMilliseconds();
    });
}

u16 GhostManagerPage::SPList::count() const {
    return m_count;
}

const u16 *GhostManagerPage::SPList::indices() const {
    return m_indices;
}

void GhostManagerPage::process() {
    if (m_currentRequest == Request::Populate) {
        processPopulate();
    }
    bool nextRequestIsPopulate = m_nextRequest == Request::Populate;

    REPLACED(process)();

    if (nextRequestIsPopulate) {
        processPopulate();
    }
}

void GhostManagerPage::requestPopulate() {
    m_nextRequest = Request::Populate;
}

void GhostManagerPage::processPopulate() {
    if (System::SaveManager::Instance()->isBusy()) {
        return;
    }

    m_list.populate();

    m_currentRequest = Request::None;
}

void GhostManagerPage::repopulate() {
    m_list.populate();
}

const GhostManagerPage::SPList *GhostManagerPage::list() const {
    return &m_list;
}

void GhostManagerPage::setupGhostReplay(bool /* isStaffGhost */) {
    setupTimeAttack(false, false);
}

void GhostManagerPage::setupGhostRace(bool /* isStaffGhost */, bool /* isNewRecord */,
        bool fromReplay) {
    setupTimeAttack(true, fromReplay);
}

void GhostManagerPage::setupTimeAttack(bool isRace, bool fromReplay) {
    auto *context = SectionManager::Instance()->globalContext();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (!isRace) {
        context->m_playerMiis.copy(&context->m_localPlayerMiis, 0, context->m_timeAttackGhostCount);
    }
    for (u32 i = context->m_timeAttackGhostCount; i-- > 0;) {
        menuScenario.players[i + !!isRace].type = System::RaceConfig::Player::Type::Ghost;
        if (fromReplay) {
            context->m_playerMiis.swap(i, i + 1);
        } else {
            auto *header = reinterpret_cast<const System::RawGhostHeader *>(
                    (*menuScenario.ghostBuffer)[i]);
            context->m_playerMiis.insertFromRaw(i + !!isRace, &header->mii);
        }
    }
    menuScenario.courseId = context->m_timeAttackCourseId;
    context->copyPlayerMiis();
    menuScenario.engineClass = System::RaceConfig::EngineClass::CC150;
    menuScenario.gameMode = System::RaceConfig::GameMode::TimeAttack;
    menuScenario.cameraMode = 0;
    menuScenario.mirror = false;
    menuScenario.teams = false;
}

} // namespace UI
