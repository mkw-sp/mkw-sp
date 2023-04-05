#include "CourseSelectPage.hh"

#include "game/system/RaceConfig.hh"
#include "game/ui/RaceConfirmPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/VotingBackPage.hh"

#include <sp/ScopeLock.hh>
#include <sp/TrackPackManager.hh>
#include <vendor/magic_enum/magic_enum.hpp>

#include <algorithm>

namespace UI {

CourseSelectPage::CourseSelectPage() = default;

CourseSelectPage::~CourseSelectPage() = default;

SP::TrackGameMode getTrackGameMode() {
    using enum System::RaceConfig::GameMode;

    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    if (menuScenario.gameMode == OfflineVS || menuScenario.gameMode == TimeAttack) {
        return SP::TrackGameMode::Race;
    } else if (menuScenario.gameMode == OfflineBT) {
        if (menuScenario.battleType == 0) {
            return SP::TrackGameMode::Balloon;
        } else {
            return SP::TrackGameMode::Coin;
        }
    } else {
        panic("Unknown gamemode!");
    }
}

PageId CourseSelectPage::getReplacement() {
    return m_replacement;
}

void CourseSelectPage::onInit() {
    m_sheetIndex = 0;
    m_sheetCount = 1;
    m_lastSelected = 0;

    m_inputManager.init(0x1, false);
    setInputManager(&m_inputManager);
    m_inputManager.setWrappingMode(MultiControlInputManager::WrappingMode::Neither);

    initChildren(5 + m_buttons.size());
    insertChild(0, &m_pageTitleText, 0);
    insertChild(1, &m_sheetSelect, 0);
    insertChild(2, &m_sheetLabel, 0);
    insertChild(3, &m_scrollBar, 0);
    insertChild(4, &m_backButton, 0);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        insertChild(5 + i, &m_buttons[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < m_buttons.size(); i++) {
        m_buttons[i].load(i);
    }
    m_sheetSelect.load("button", "CourseSelectArrowRight", "ButtonArrowRight",
            "CourseSelectArrowLeft", "ButtonArrowLeft", 0x1, false, false);
    m_sheetLabel.load("control", "TimeAttackGhostListPageNum", "TimeAttackGhostListPageNum", NULL);
    m_scrollBar.load(m_sheetCount, m_sheetIndex, "button", "CourseSelectScrollBar",
            "CourseSelectScrollBar", 0x1, false, false);
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
    m_backConfirmed = false;

    for (u32 i = 0; i < m_buffers.size(); i++) {
        for (u8 c = 0; c < m_buffers[i].size(); c++) {
            m_buffers[i][c].reset(new (0x20) u8[MaxThumbnailHeight * MaxThumbnailWidth]);
        }
    }
}

void CourseSelectPage::onActivate() {
    m_request = Request::None;
    OSInitThreadQueue(&m_queue);
    u8 *stackTop = m_stack + sizeof(m_stack);
    OSCreateThread(&m_thread, LoadThumbnails, this, stackTop, sizeof(m_stack), 24, 0);
    OSResumeThread(&m_thread);

    m_replacement = PageId::None;

    auto &packManager = SP::TrackPackManager::Instance();
    auto &pack = packManager.getSelectedTrackPack();

    m_sheetIndex = 0;
    m_lastSelected = 0;
    m_sheetCount = (pack.getTrackCount(getTrackGameMode()) + 9 - 1) / 9;
    m_scrollBar.reconfigure(m_sheetCount, m_sheetIndex, m_sheetCount >= 4 ? 0x1 : 0x0);

    m_sheetSelect.setVisible(m_sheetCount > 1);
    m_sheetSelect.setPlayerFlags(m_sheetCount <= 1 ? 0x0 : 0x1);

    refresh();

    m_buttons[0].selectDefault(0);
}

void CourseSelectPage::onDeactivate() {
    m_request = Request::Stop;
    OSWakeupThread(&m_queue);
    OSJoinThread(&m_thread, nullptr);
    OSDetachThread(&m_thread);
}

void CourseSelectPage::afterCalc() {
    SP::ScopeLock<SP::NoInterrupts> lock;
    bool changed = false;
    for (size_t i = 0; i < m_buttons.size(); i++) {
        if (m_thumbnailChanged[i]) {
            for (u8 c = 0; c < m_texObjs[i].size(); c++) {
                m_buttons[i].setTex(c, m_texObjs[i][c]);
            }
            m_thumbnailChanged[i] = false;
            m_buttons[i].setPaneVisible("picture_base", true);
            changed = true;
        }
    }
    if (changed) {
        GXInvalidateTexAll();
    }
}

void CourseSelectPage::onRefocus() {
    auto *section = SectionManager::Instance()->currentSection();
    auto sectionId = section->id();

    if (Section::HasRoomClient(sectionId)) {
        return;
    }

    if (m_backConfirmed) {
        changeSection(SectionId::TitleFromMenu, Anim::Prev, 0.0f);
        return;
    }

    auto *raceConfirmPage = section->page<PageId::RaceConfirm>();
    if (raceConfirmPage->hasConfirmed()) {
        auto &menuScenario = System::RaceConfig::Instance()->menuScenario();

        if (menuScenario.gameMode == System::RaceConfig::GameMode::OfflineBT) {
            changeSection(SectionId::BTDemo, Anim::Next, 0.0f);
        } else {
            changeSection(SectionId::VSDemo, Anim::Next, 0.0f);
        }
    }
}

u32 CourseSelectPage::sheetIndex() const {
    return m_sheetIndex;
}

u32 CourseSelectPage::lastSelected() const {
    return m_lastSelected;
}

void CourseSelectPage::onBack(u32 /* localPlayerId */) {
    onBackCommon(0.0f);
}

void CourseSelectPage::onButtonFront(PushButton *button, u32 /* localPlayerId */) {
    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    auto sectionId = section->id();

    auto databaseId = *m_databaseIds[button->m_index];
    if (Section::HasRoomClient(sectionId)) {
        // TODO(GnomedDev): Fix online voting.
        // auto *votingBackPage = section->page<PageId::VotingBack>();
        // votingBackPage->setLocalVote(databaseId);
        // votingBackPage->setSubmitted(true);
        startReplace(Anim::Next, button->getDelay());
    } else {
        auto raceConfig = System::RaceConfig::Instance();
        raceConfig->m_packInfo.selectCourse(databaseId);

        if (raceConfig->menuScenario().gameMode == System::RaceConfig::GameMode::TimeAttack) {
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
        if (sectionId == SectionId::Multi) {
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
    auto gameMode = getTrackGameMode();

    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &trackPack = trackPackManager.getSelectedTrackPack();
    auto trackCount = trackPack.getTrackCount(gameMode);

    {
        SP::ScopeLock<SP::NoInterrupts> lock;

        s32 start = (-1 + m_sheetIndex) * m_buttons.size();
        s32 end = start + 3 * m_buttons.size();
        size_t start_offset = 0;
        s32 end_offset = 0;

        if (start < 0 && end < trackCount) {
            start = trackCount + start;
            start_offset = std::max(end - start, 0);
        } else if (start >= 0 && end > trackCount) {
            end = end - trackCount;
            end_offset = std::min(start - end, 0);
        } else if (start < 0 && end >= trackCount) {
            start_offset = -start;
            end_offset = trackCount - end;
            start = 0;
            end = trackCount;
        }

        for (size_t i = 0; i < 3 * m_buttons.size(); i++) {
            size_t j = i;
            if (j < m_buttons.size()) {
                j += m_buttons.size();
            } else if (j < 2 * m_buttons.size()) {
                j -= m_buttons.size();
            }
            if (i >= start_offset && i < 3 * m_buttons.size() + end_offset) {
                auto trackIndex = (start + i - start_offset) % trackCount;
                m_databaseIds[j] = trackPack.getNthTrack(trackIndex, gameMode).value();
            } else {
                m_databaseIds[j] = std::nullopt;
            }
        }

        for (size_t i = 0; i < m_buttons.size(); i++) {
            auto courseIndex = m_sheetIndex * m_buttons.size() + i;
            if (m_databaseIds[i].has_value() && courseIndex < trackCount) {
                m_buttons[i].setVisible(true);
                m_buttons[i].setPlayerFlags(1 << 0);
                m_buttons[i].refresh(*m_databaseIds[i]);
            } else {
                m_buttons[i].setVisible(false);
                m_buttons[i].setPlayerFlags(0);
            }
        }

        m_request = Request::Change;
        OSWakeupThread(&m_queue);
    }

    m_sheetLabel.setVisible(m_sheetCount != 0);
    MessageInfo info{};
    info.intVals[0] = m_sheetIndex + 1;
    info.intVals[1] = m_sheetCount;
    m_sheetLabel.setMessageAll(2009, &info);
}

void CourseSelectPage::loadThumbnails() {
    std::array<std::optional<Sha1>, 27> databaseIds;

    while (true) {
        std::array<std::optional<Sha1>, 27> requestedDatabaseIds{};
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

        // Handle thumbnails that are already loaded
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
            requestedDatabaseIds[i] = std::nullopt;
            std::swap(databaseIds[i], databaseIds[j]);
            std::swap(m_buffers[i], m_buffers[j]);
            std::swap(m_texObjs[i], m_texObjs[j]);
            m_thumbnailChanged[i] = true;
        }

        // Load new thumbnails
        for (u32 i = 0; i < requestedDatabaseIds.size(); i++) {
            auto databaseId = requestedDatabaseIds[i];
            if (!databaseId.has_value()) {
                continue;
            }

            JRESULT result = loadThumbnail(i, *databaseId);
            if (result == JDR_OK) {
                databaseIds[i] = *databaseId;
            } else {
                databaseIds[i] = std::nullopt;
            }

            SP::ScopeLock<SP::NoInterrupts> lock;
            if (m_request != Request::None) {
                break;
            }

            if (result == JDR_OK) {
                m_thumbnailChanged[i] = true;
            } else {
                auto hex = sha1ToHex(*databaseId);
                SP_LOG("Failed to read thumbnail %s with error %u", hex, result);
            }
        }
    }
}

JRESULT CourseSelectPage::loadThumbnail(u32 i, Sha1 databaseId) {
    char path[128];

    auto &trackPackInfo = System::RaceConfig::Instance()->m_packInfo;
    auto hex = sha1ToHex(databaseId);
    if (trackPackInfo.isVanilla()) {
        snprintf(path, std::size(path), "/thumbnails/%s.jpg", hex.data());
    } else {
        snprintf(path, std::size(path), "/mkw-sp/Track Thumbnails/%s.jpg", hex.data());
    }

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
