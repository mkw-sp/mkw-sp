#include "GhostManagerPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

#include <algorithm>
#include <cstring>

namespace UI {

void GhostManagerPage::List::populate(u32 UNUSED(courseId)) {}

void GhostManagerPage::SPList::populate() {
    auto *saveManager = System::SaveManager::Instance();
    u32 courseId = System::RaceConfig::Instance()->menuScenario().courseId;
    const u8 *courseSHA1 = saveManager->courseSHA1(courseId);
    auto cc = saveManager->getSetting<SP::ClientSettings::Setting::TAClass>();
    bool speedModIsEnabled = cc == SP::ClientSettings::TAClass::CC200;
    m_count = 0;
    for (u32 i = 0; i < saveManager->ghostCount(); i++) {
        auto *header = saveManager->rawGhostHeader(i);
        auto *footer = saveManager->ghostFooter(i);
        if (footer->courseSHA1() && memcmp(*(footer->courseSHA1()), courseSHA1, 0x14)) {
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
        auto *t0 = &h0->raceTime;
        auto *t1 = &h1->raceTime;
        u32 u0 = (t0->minutes * 60 + t0->seconds) * 1000 + t0->milliseconds;
        u32 u1 = (t1->minutes * 60 + t1->seconds) * 1000 + t1->milliseconds;
        u32 d0 = (h0->year * 12 + h0->month) * 31 + h0->day;
        u32 d1 = (h1->year * 12 + h1->month) * 31 + h1->day;
        switch (saveManager->getSetting<SP::ClientSettings::Setting::TAGhostSorting>()) {
        case SP::ClientSettings::TAGhostSorting::Fastest:
            return u0 < u1;
        case SP::ClientSettings::TAGhostSorting::Slowest:
            return u0 > u1;
        case SP::ClientSettings::TAGhostSorting::Newest:
            return d0 > d1;
        case SP::ClientSettings::TAGhostSorting::Oldest:
            return d0 < d1;
        default:
            assert(false);
        }
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
        dispatchPopulate();
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

void GhostManagerPage::dispatchPopulate() {
    u32 courseId = System::RaceConfig::Instance()->menuScenario().courseId;
    if (System::SaveManager::Instance()->computeCourseSHA1Async(courseId)) {
        processPopulate();
        return;
    }

    m_currentRequest = Request::Populate;
}

const GhostManagerPage::SPList *GhostManagerPage::list() const {
    return &m_list;
}

void GhostManagerPage::setupGhostReplay(bool UNUSED(isStaffGhost)) {
    setupTimeAttack(false, false);
}

void GhostManagerPage::setupGhostRace(bool UNUSED(isStaffGhost), bool UNUSED(isNewRecord),
        bool fromReplay) {
    setupTimeAttack(true, fromReplay);
}

void GhostManagerPage::setupTimeAttack(bool isRace, bool fromReplay) {
    auto *context = SectionManager::Instance()->globalContext();
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (!isRace) {
        context->m_playerMiis.copy(&context->m_localPlayerMiis, 0, context->m_timeAttackGhostCount);
    }
    for (u32 i = context->m_timeAttackGhostCount; i --> 0;) {
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
