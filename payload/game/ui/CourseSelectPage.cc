#include "CourseSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/system/SaveManager.hh"
#include "game/ui/ModelRenderPage.hh"
#include "game/ui/RaceConfirmPage.hh"
#include "game/ui/RankingPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/VotingBackPage.hh"
#include "game/ui/model/MenuModelManager.hh"

#include <sp/ScopeLock.hh>

#include <algorithm>
#include <limits>

namespace UI {

CourseSelectPage::CourseSelectPage() = default;

CourseSelectPage::~CourseSelectPage() = default;

PageId CourseSelectPage::getReplacement() {
    return m_replacement;
}

void CourseSelectPage::onInit() {
    bool isSPRankingsSection =
            SectionManager::Instance()->currentSection()->id() == SectionId::ServicePackRankings;

    m_filter = {false, false};
    m_sheetCount = 1;
    m_sheetIndex = 0;
    m_lastSelected = 0;

    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(5 + m_buttons.size() + isSPRankingsSection);
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_sheetSelect, 0);
    insertChild(2, &m_sheetLabel, 0);
    insertChild(3, &m_scrollBar, 0);
    insertChild(4, &m_backButton, 0);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        insertChild(5 + i, &m_buttons[i], 0);
    }
    if (isSPRankingsSection) {
        m_blackBackControl = std::make_unique<BlackBackControl>();
        insertChild(5 + m_buttons.size(), m_blackBackControl.get(), 0);
        m_blackBackControl->load("control", "RankingBlackBack", "RankingBlackBack");
        m_blackBackControl->m_zIndex = -1.0f;
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        m_buttons[i].load(i);
    }

    const char *buttonArrowRightVariant;
    const char *buttonArrowLeftVariant;
    const char *courseSelectPageNumVariant;
    const char *courseSelectScrollBarVariant;
    if (isSPRankingsSection) {
        buttonArrowRightVariant = "RankingButtonArrowRight";
        buttonArrowLeftVariant = "RankingButtonArrowLeft";
        courseSelectPageNumVariant = "RankingCourseSelectPageNum";
        courseSelectScrollBarVariant = "RankingCourseSelectScrollBar";
    } else {
        buttonArrowRightVariant = "ButtonArrowRight";
        buttonArrowLeftVariant = "ButtonArrowLeft";
        courseSelectPageNumVariant = "CourseSelectPageNum";
        courseSelectScrollBarVariant = "CourseSelectScrollBar";
    }

    m_sheetSelect.load("button", "CourseSelectArrowRight", buttonArrowRightVariant,
            "CourseSelectArrowLeft", buttonArrowLeftVariant, 0x1, false, false);
    m_sheetLabel.load("control", "CourseSelectPageNum", courseSelectPageNumVariant, NULL);
    m_scrollBar.load(m_sheetCount, m_sheetIndex, "button", "CourseSelectScrollBar",
            courseSelectScrollBarVariant, 0x1, false, false);
    m_backButton.load("button", "Back", "ButtonBack", 0x1, false, true);

    m_inputManager.setHandler(MenuInputManager::InputId::Back, &m_onBack, false, false);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        m_buttons[i].setFrontHandler(&m_onButtonFront, false);
        m_buttons[i].setSelectHandler(&m_onButtonSelect, false);
    }
    m_sheetSelect.setRightHandler(&m_onSheetSelectRight);
    m_sheetSelect.setLeftHandler(&m_onSheetSelectLeft);
    m_scrollBar.setChangeHandler(&m_onScrollBarChange);
    m_backButton.setFrontHandler(&m_onBackButtonFront, false);

    m_pageTitleText.setMessage(3405);

    m_sheetSelect.setVisible(m_sheetCount > 1);
    m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

    m_backConfirmed = false;

    for (u32 i = 0; i < m_buffers.size(); i++) {
        for (u8 c = 0; c < m_buffers[i].size(); c++) {
            m_buffers[i][c].reset(new (0x20) u8[MaxThumbnailHeight * MaxThumbnailWidth]);
        }
    }
    m_request = Request::None;
    OSInitThreadQueue(&m_queue);
    u8 *stackTop = m_stack + sizeof(m_stack);
    OSCreateThread(&m_thread, LoadThumbnails, this, stackTop, sizeof(m_stack), 24, 0);
    OSResumeThread(&m_thread);

    auto sectionId = SectionManager::Instance()->currentSection()->id();
    switch (sectionId) {
    case SectionId::SingleChangeDriver:
    case SectionId::SingleChangeCourse:
    case SectionId::SingleSelectVSCourse:
    case SectionId::SingleSelectBTCourse:
    case SectionId::SingleChangeGhostData:
    case SectionId::Voting1PVS:
    case SectionId::Voting2PVS:
    case SectionId::ServicePackRankings:
        filter();
        break;
    default: {
        std::string_view sectionName = magic_enum::enum_name(sectionId);
        SP_LOG("Not refreshing on section '%.*s' (0x%X)", sectionName.size(), sectionName.data(),
                static_cast<u32>(sectionId));
    }
    }
}

void CourseSelectPage::onDeinit() {
    m_request = Request::Stop;
    OSWakeupThread(&m_queue);
    OSJoinThread(&m_thread, nullptr);
    OSDetachThread(&m_thread);
}

void CourseSelectPage::onActivate() {
    m_replacement = PageId::None;

    auto *sectionManager = SectionManager::Instance();
    auto section = sectionManager->currentSection()->id();
    if (section != SectionId::SingleSelectVSCourse && section != SectionId::SingleSelectBTCourse) {
        sectionManager->globalContext()->clearCourses();
    }
}

void CourseSelectPage::afterCalc() {
    SP::ScopeLock<SP::NoInterrupts> lock;
    bool changed = false;
    for (size_t i = 0; i < m_buttons.size(); i++) {
        if (m_thumbnailChanged[i]) {
            for (u8 c = 0; c < m_texObjs[i].size(); c++) {
                m_buttons[i].refresh(c, m_texObjs[i][c]);
            }
            m_thumbnailChanged[i] = false;
            m_buttons[i].setPaneVisible("picture_base", true);
            changed = true;
        }
    }
    if (changed) {
        GXInvalidateTexAll();
    }

    // When we reach the DriftSelectPage the first time in one of these sections, no vehicle is
    // selected for the model. Vehicles that are not selected (here, all of them) are constantly
    // rotating, thus we would get an unexpected spin animation as the NoteModelControl of the
    // DriftSelectPage restores the selection from GlobalContext. We avoid this by restoring the
    // selection manually in this function.
    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();
    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();
    if (sectionId == SectionId::SingleChangeCourse ||
            sectionId == SectionId::SingleChangeGhostData) {
        auto *modelRenderPage = section->page<PageId::ModelRender>();
        modelRenderPage->configure(0, true, true);
        modelRenderPage->setCharacterId(0, globalContext->m_localCharacterIds[0]);
        modelRenderPage->setVehicleId(0, globalContext->m_localVehicleIds[0]);
        auto *driverModelManager = MenuModelManager::Instance()->driverModelManager();
        auto *model = driverModelManager->handle(0)->model;
        model->m_vehicleId = globalContext->m_localVehicleIds[0];
        model->setAnim(0, 3);
    }
}

void CourseSelectPage::onRefocus() {
    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    if (Section::HasRoomClient(section->id())) {
        return;
    }

    if (m_backConfirmed) {
        changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
        return;
    }

    auto *raceConfig = System::RaceConfig::Instance();
    auto *raceConfirmPage = section->page<PageId::RaceConfirm>();
    if (raceConfirmPage->hasConfirmed()) {
        if (raceConfig->menuScenario().gameMode == System::RaceConfig::GameMode::OfflineBT) {
            changeSection(SectionId::BTDemo, Anim::Next, 0.0f);
        } else {
            changeSection(SectionId::VSDemo, Anim::Next, 0.0f);
        }
    } else {
        sectionManager->globalContext()->clearCourses();
    }
}

u32 CourseSelectPage::sheetIndex() const {
    return m_sheetIndex;
}

u32 CourseSelectPage::lastSelected() const {
    return m_lastSelected;
}

// This function must be called before the 'calc' function is called
void CourseSelectPage::filter() {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    SP::CourseDatabase::Filter defaultFilter = {menuScenario.isVs(), menuScenario.isBattle()};
    filter(defaultFilter);
}

// This function must be called before the 'calc' function is called
void CourseSelectPage::filter(const SP::CourseDatabase::Filter &filter) {
    if (filter.race == m_filter.race && filter.battle == m_filter.battle) {
        return;
    }
    m_filter = filter;

    auto &courseDatabase = SP::CourseDatabase::Instance();
    m_sheetCount = (courseDatabase.count(m_filter) + m_buttons.size() - 1) / m_buttons.size();
    if (auto selection = courseDatabase.loadSelection()) {
        m_sheetIndex = *selection / m_buttons.size();
        m_lastSelected = *selection % m_buttons.size();
    } else {
        m_sheetIndex = 0;
        m_lastSelected = 0;
    }
    m_scrollBar.reconfigure(m_sheetCount, m_sheetIndex, m_sheetCount >= 4 ? 0x1 : 0x0);

    m_sheetSelect.setVisible(m_sheetCount > 1);
    m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

    refresh();

    m_buttons[m_lastSelected].selectDefault(0);
}

void CourseSelectPage::refreshSelection(u32 selection) {
    auto &courseDatabase = SP::CourseDatabase::Instance();
    courseDatabase.saveSelection(selection);

    m_sheetIndex = selection / m_buttons.size();
    m_lastSelected = selection % m_buttons.size();

    m_scrollBar.reconfigure(m_sheetCount, m_sheetIndex, m_sheetCount >= 4 ? 0x1 : 0x0);

    refresh();

    m_buttons[m_lastSelected].selectDefault(0);
}

void CourseSelectPage::onBack(u32 /* localPlayerId */) {
    onBackCommon(0.0f);
}

void CourseSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    u32 courseIndex = m_sheetIndex * m_buttons.size() + button->m_index;
    auto &courseDatabase = SP::CourseDatabase::Instance();
    auto &entry = courseDatabase.entry(m_filter, courseIndex);
    courseDatabase.saveSelection(courseIndex);

    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();

    if (Section::HasRoomClient(sectionId)) {
        auto *votingBackPage = section->page<PageId::VotingBack>();
        votingBackPage->setLocalVote(entry.courseId);
        votingBackPage->setSubmitted(true);
        startReplace(Anim::Next, button->getDelay());
    } else if (sectionId == SectionId::ServicePackRankings) {
        s32 courseButtonIndex = GetButtonIndexFromCourse(entry.courseId);

        auto *rankingPage = section->page<PageId::Ranking>();
        rankingPage->courseControl().choose(courseButtonIndex);

        m_replacement = PageId::None;
        startReplace(Anim::Next, button->getDelay());
    } else {
        auto *globalContext = sectionManager->globalContext();
        auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
        auto &spMenu = System::RaceConfig::Instance()->m_spMenu;

        if (!sectionManager->globalContext()->generateOrderedCourses(courseIndex)) {
            globalContext->setCurrentCourse(entry.courseId);
            menuScenario.courseId = entry.courseId;
            spMenu.pathReplacement.m_len = 0;
            spMenu.musicReplacement = std::nullopt;
            spMenu.courseSha = std::nullopt;
            spMenu.nameReplacement.m_len = 0;
        }

        if (menuScenario.gameMode == System::RaceConfig::GameMode::TimeAttack) {
            m_replacement = PageId::TimeAttackTop;
            startReplace(Anim::Next, button->getDelay());
        } else {
            push(PageId::RaceConfirm, Anim::Next);
        }
    }
}

void CourseSelectPage::onButtonSelect(PushButton *button, u32 /* localPlayerId */) {
    m_lastSelected = button->m_index;
}

void CourseSelectPage::onSheetSelectRight(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
    if (m_sheetIndex == m_sheetCount - 1) {
        m_sheetIndex = 0;
    } else {
        ++m_sheetIndex;
    }

    refresh();

    m_lastSelected = m_lastSelected / 3 * 3 + 0;
    while (!m_buttons[m_lastSelected].getVisible()) {
        --m_lastSelected;
    }
    m_buttons[m_lastSelected].select(0);

    m_scrollBar.m_chosen = m_sheetIndex;
}

void CourseSelectPage::onSheetSelectLeft(SheetSelectControl * /* control */,
        u32 /* localPlayerId */) {
    if (m_sheetIndex == 0) {
        m_sheetIndex = m_sheetCount - 1;
    } else {
        --m_sheetIndex;
    }

    refresh();

    m_lastSelected = m_lastSelected / 3 * 3 + 2;
    while (!m_buttons[m_lastSelected].getVisible()) {
        --m_lastSelected;
    }
    m_buttons[m_lastSelected].select(0);

    m_scrollBar.m_chosen = m_sheetIndex;
}

void CourseSelectPage::onScrollBarChange(ScrollBar * /* scrollBar */, u32 /* localPlayerId */,
        u32 chosen) {
    m_sheetIndex = chosen;

    refresh();
}

void CourseSelectPage::onBackButtonFront(PushButton *button, u32 /* localPlayerId */) {
    f32 delay = button->getDelay();
    onBackCommon(delay);
}

void CourseSelectPage::onBackConfirm(s32 /* choice */, PushButton * /* button */) {
    m_backConfirmed = true;
}

void CourseSelectPage::onBackCommon(f32 delay) {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();
    u32 backMessageId;

    if (Section::HasRoomClient(sectionId)) {
        return;
    } else if (sectionId == SectionId::SingleSelectVSCourse) {
        backMessageId = 3470;
    } else if (sectionId == SectionId::SingleSelectBTCourse) {
        backMessageId = 3471;
    } else {
        if (sectionId == SectionId::ServicePackRankings) {
            m_replacement = PageId::None;
        } else if (sectionId == SectionId::Multi) {
            m_replacement = PageId::MultiDriftSelect;
        } else {
            m_replacement = PageId::DriftSelect;
        }
        startReplace(Anim::Prev, delay);
        return;
    }

    auto *yesNoPage = section->page<PageId::YesNoPopup>();
    yesNoPage->reset();
    yesNoPage->setWindowMessage(backMessageId, nullptr);
    yesNoPage->configureButton(0, 2002, nullptr, Anim::None, &m_onBackConfirm);
    yesNoPage->configureButton(1, 2003, nullptr, Anim::None, nullptr);
    yesNoPage->setDefaultChoice(1);
    push(PageId::YesNoPopup, Anim::Next);
}

void CourseSelectPage::refresh() {
    {
        SP::ScopeLock<SP::NoInterrupts> lock;
        s32 count = SP::CourseDatabase::Instance().count(m_filter);
        s32 start = (-1 + m_sheetIndex) * m_buttons.size();
        s32 end = start + 3 * m_buttons.size();
        s32 start_offset = 0;
        s32 end_offset = 0;
        if (start < 0 && end < count) {
            start = count + start;
            start_offset = std::max(end - start, 0);
        } else if (start >= 0 && end > count) {
            end = end - count;
            end_offset = std::min(start - end, 0);
        } else if (start < 0 && end >= count) {
            start_offset = -start;
            end_offset = count - end;
            start = 0;
            end = count;
        }
        for (s32 i = 0; i < 3 * static_cast<s32>(m_buttons.size()); i++) {
            size_t j = i;
            if (j < m_buttons.size()) {
                j += m_buttons.size();
            } else if (j < 2 * m_buttons.size()) {
                j -= m_buttons.size();
            }
            if (i >= start_offset && i < 3 * static_cast<s32>(m_buttons.size()) + end_offset) {
                auto &entry = SP::CourseDatabase::Instance().entry(m_filter,
                        (start + i - start_offset) % count);
                m_databaseIds[j] = entry.databaseId;
            } else {
                m_databaseIds[j] = std::numeric_limits<u32>::max();
            }
        }
        for (size_t i = 0; i < m_buttons.size(); i++) {
            if (m_databaseIds[i] != std::numeric_limits<u32>::max() &&
                    m_sheetIndex * m_buttons.size() + i < static_cast<size_t>(count)) {
                m_buttons[i].setVisible(true);
                m_buttons[i].setPlayerFlags(1 << 0);
            } else {
                m_buttons[i].setVisible(false);
                m_buttons[i].setPlayerFlags(0);
            }
        }
        m_request = Request::Change;
        OSWakeupThread(&m_queue);
    }

    for (size_t i = 0; i < m_buttons.size(); i++) {
        u32 courseIndex = m_sheetIndex * m_buttons.size() + i;
        if (courseIndex < SP::CourseDatabase::Instance().count(m_filter)) {
            auto &entry = SP::CourseDatabase::Instance().entry(m_filter, courseIndex);
            auto courseId = static_cast<u32>(entry.courseId);

            m_buttons[i].refresh(courseId < 32 ? 9300 + courseId : 9400 - 32 + courseId);
        }
    }

    m_sheetLabel.setVisible(m_sheetCount != 0);
    MessageInfo info{};
    info.intVals[0] = m_sheetIndex + 1;
    info.intVals[1] = m_sheetCount;
    m_sheetLabel.setMessageAll(2009, &info);
}

void CourseSelectPage::loadThumbnails() {
    std::array<u32, 27> databaseIds;
    std::fill(databaseIds.begin(), databaseIds.end(), std::numeric_limits<u32>::max());

    while (true) {
        std::array<u32, 27> requestedDatabaseIds{};
        {
            SP::ScopeLock<SP::NoInterrupts> lock;
            switch (m_request) {
            case Request::None:
                OSSleepThread(&m_queue);
                continue;
            case Request::Stop:
                return;
            default:
                break;
            }
            requestedDatabaseIds = m_databaseIds;
            m_request = Request::None;
        }

        for (u32 i = 0; i < requestedDatabaseIds.size(); i++) {
            SP::ScopeLock<SP::NoInterrupts> lock;
            auto it = std::find(databaseIds.begin(), databaseIds.end(), requestedDatabaseIds[i]);
            if (it == databaseIds.end()) {
                if (i < m_buttons.size()) {
                    m_buttons[i].setPaneVisible("picture_base", false);
                }
                continue;
            }
            auto j = std::distance(databaseIds.begin(), it);
            requestedDatabaseIds[i] = std::numeric_limits<u32>::max();
            std::swap(databaseIds[i], databaseIds[j]);
            std::swap(m_buffers[i], m_buffers[j]);
            std::swap(m_texObjs[i], m_texObjs[j]);
            m_thumbnailChanged[i] = true;
        }

        for (u32 i = 0; i < requestedDatabaseIds.size(); i++) {
            if (requestedDatabaseIds[i] == std::numeric_limits<u32>::max()) {
                continue;
            }

            auto &courseDatabase = SP::CourseDatabase::Instance();

            std::optional<Sha1> courseSha1 = std::nullopt;
            for (u32 courseIdx = 0; courseIdx < courseDatabase.count(m_filter); courseIdx += 1) {
                auto entry = courseDatabase.entry(m_filter, courseIdx);
                if (entry.databaseId == requestedDatabaseIds[i]) {
                    courseSha1 = System::SaveManager::Instance()->courseSHA1(entry.courseId);
                }
            }

            JRESULT result = loadThumbnail(i, courseSha1.value());
            if (result == JDR_OK) {
                databaseIds[i] = requestedDatabaseIds[i];
            } else {
                databaseIds[i] = std::numeric_limits<u32>::max();
            }
            SP::ScopeLock<SP::NoInterrupts> lock;
            if (m_request != Request::None) {
                break;
            }
            if (result == JDR_OK) {
                m_thumbnailChanged[i] = true;
            } else {
                SP_LOG("Failed to read thumbnail with error %u", result);
            }
        }
    }
}

JRESULT CourseSelectPage::loadThumbnail(u32 i, Sha1 courseSha1) {
    char path[64];

    auto hex = sha1ToHex(courseSha1);
    snprintf(path, std::size(path), "/thumbnails/%s.jpg", hex.data());

    auto file = SP::Storage::OpenRO(path);
    if (!file) {
        return JDR_INP;
    }

    Context context{this, i, std::move(*file), 0};

    u8 buffer[0x3000 /* 12 KiB */];
    JDEC jdec;
    if (auto result = jd_prepare(&jdec, ReadCompressedThumbnail, buffer, sizeof(buffer), &context);
            result != JDR_OK) {
        return result;
    }

    if (jdec.width > MaxThumbnailWidth || jdec.height > MaxThumbnailHeight) {
        return JDR_FMT2;
    }

    if (auto result = jd_decomp(&jdec, WriteUncompressedThumbnail, 0); result != JDR_OK) {
        return result;
    }

    for (u8 c = 0; c < m_buffers[i].size(); c++) {
        DCFlushRange(m_buffers[i][c].get(), MaxThumbnailHeight * MaxThumbnailWidth);
        GXInitTexObj(&m_texObjs[i][c], m_buffers[i][c].get(), jdec.width, jdec.height, GX_TF_I8,
                GX_CLAMP, GX_CLAMP, GX_FALSE);
    }

    return JDR_OK;
}

void *CourseSelectPage::LoadThumbnails(void *arg) {
    reinterpret_cast<CourseSelectPage *>(arg)->loadThumbnails();
    return nullptr;
}

size_t CourseSelectPage::ReadCompressedThumbnail(JDEC *jdec, uint8_t *buffer, size_t size) {
    auto *context = reinterpret_cast<Context *>(jdec->device);

    if (context->offset + size > context->file.size()) {
        size = context->file.size() - context->offset;
    }

    if (buffer) {
        if (!context->file.read(buffer, size, context->offset)) {
            return 0;
        }
    }

    context->offset += size;
    return size;
}

int CourseSelectPage::WriteUncompressedThumbnail(JDEC *jdec, void *bitmap, JRECT *rect) {
    auto *context = reinterpret_cast<Context *>(jdec->device);

    auto *pixels = reinterpret_cast<const u8 *>(bitmap);
    u16 bwidth = jdec->width / 8;
    for (u16 y = rect->top; y <= rect->bottom; y++) {
        u16 by = y / 4;
        u16 ly = y % 4;
        for (u16 x = rect->left; x <= rect->right; x++) {
            u16 bx = x / 8;
            u16 lx = x % 8;
            u32 index = (by * bwidth + bx) * (4 * 8) + ly * 8 + lx;
            for (u8 c = 0; c < context->page->m_buffers[context->i].size(); c++) {
                context->page->m_buffers[context->i][c][index] = *pixels++;
            }
        }
    }

    return context->page->m_request == Request::None;
}

} // namespace UI
