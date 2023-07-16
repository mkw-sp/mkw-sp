#include "ScreenshotManager.hh"

#include "sp/ScopeLock.hh"
#include "sp/storage/Storage.hh"

#include <egg/core/eggSystem.hh>
#include <egg/core/eggXfbManager.hh>
#include <game/sound/SceneSoundManager.hh>
#include <game/system/InputManager.hh>
#include <game/system/RaceConfig.hh>
#include <game/system/ResourceManager.hh>
#include <game/system/SaveManager.hh>
#include <game/ui/SectionManager.hh>

#include <cstring>
#include <cwchar>

namespace SP {

#define SCREENSHOT_FILE_DIRECTORY L"/mkw-sp/screenshots"
#define SCREENSHOT_FILE_EXTENSION L".xfb"
#define SCREENSHOT_FILE_EXTENSION_LENGTH (sizeof(SCREENSHOT_FILE_EXTENSION) / sizeof(wchar_t) - 1)

ScreenshotManager *ScreenshotManager::s_instance = nullptr;

ScreenshotManager::ScreenshotManager(u32 framebufferSize) {
    m_framebufferInfo.framebuffer =
            new (EGG::TSystem::Instance().eggRootMEM2(), 32) u8[framebufferSize];
    m_framebufferInfo.framebufferSize = framebufferSize;
}

void ScreenshotManager::calc() {
    auto *saveManager = System::SaveManager::Instance();
    bool inputBoundToY = saveManager->getSetting<SP::ClientSettings::Setting::YButton>() ==
            SP::ClientSettings::YButton::Screenshot;
    if (inputBoundToY && !m_saving && m_colorFader.getStatus() == EGG::ColorFader::Status::Hidden) {
        auto *inputManager = System::InputManager::Instance();
        for (int i = 0; i < 4; i++) {
            PADStatus padStatus = inputManager->padStatus(i);
            if (padStatus.error != PAD_ERROR_NONE) {
                continue;
            }
            if (padStatus.buttons & PAD_BUTTON_Y) {
                capture(EGG::TSystem::Instance().getXfbManager()->headXfb());
                break;
            }
        }
    }

    m_colorFader.calc();
}

void ScreenshotManager::draw() {
    m_colorFader.draw();
}

ScreenshotManager *ScreenshotManager::CreateInstance(u32 framebufferSize) {
    assert(!s_instance);
    assert(Storage::CreateDir(SCREENSHOT_FILE_DIRECTORY, true));

    s_instance =
            new (EGG::TSystem::Instance().eggRootMEM2(), 32) ScreenshotManager(framebufferSize);
    return s_instance;
}

ScreenshotManager *ScreenshotManager::Instance() {
    return s_instance;
}

void ScreenshotManager::save() {
    if (Storage::WriteFile(m_screenshotFilepath.data(), m_framebufferInfo.framebuffer,
                m_framebufferInfo.framebufferSize, true)) {
        SP_LOG("Saved the screenshot to the file '%ls'!", m_screenshotFilepath.data());
    } else {
        SP_LOG("Failed to save the screenshot to the file '%ls'!", m_screenshotFilepath.data());
    }

    m_saving = false;
}

void *ScreenshotManager::SaveTask(void *arg) {
    auto *screenshotManager = reinterpret_cast<ScreenshotManager *>(arg);
    screenshotManager->save();

    return nullptr;
}

void ScreenshotManager::capture(EGG::Xfb *xfb) {
    if (m_saving) {
        return;
    }
    m_saving = true;

    u32 framebufferSize = EGG::Xfb::CalcXfbSize(xfb->width(), xfb->height());
    if (framebufferSize > m_framebufferInfo.framebufferSize) {
        m_saving = false;
        return;
    }

    {
        ScopeLock<NoInterrupts> lock;

        memcpy(m_framebufferInfo.framebuffer, xfb->buffer(), framebufferSize);
    }

    OSCalendarTime time;
    OSTicksToCalendarTime(OSGetTime(), &time);

    int charactersWritten = swprintf(m_screenshotFilepath.data(), m_screenshotFilepath.size(),
            SCREENSHOT_FILE_DIRECTORY L"/%04d-%02d-%02d-%02d-%02d-%02d" SCREENSHOT_FILE_EXTENSION,
            time.year, time.mon + 1, time.mday, time.hour, time.min, time.sec);

    auto *sectionManager = UI::SectionManager::Instance();
    if (sectionManager) {
        System::SceneId sceneId = UI::Section::GetSceneId(sectionManager->currentSection()->id());
        if (sceneId == System::SceneId::Race) {
            auto courseId = System::RaceConfig::Instance()->raceScenario().courseId;
            const char *courseFilename = System::ResourceManager::GetCourseFilename(courseId);
            swprintf(m_screenshotFilepath.data() + charactersWritten -
                            SCREENSHOT_FILE_EXTENSION_LENGTH,
                    m_screenshotFilepath.size() - charactersWritten +
                            SCREENSHOT_FILE_EXTENSION_LENGTH,
                    L"-%s" SCREENSHOT_FILE_EXTENSION, courseFilename);
        }
    }

    m_saving = OSCreateThread(&m_thread, SaveTask, this, m_stack.data() + m_stack.size(),
            m_stack.size(), 31, 0);
    if (m_saving) {
        auto *sceneSoundManager = Sound::SceneSoundManager::Instance();
        if (sceneSoundManager) {
            sceneSoundManager->play(Sound::SoundId::SE_UI_BTN_OK, -1);
        }

        m_colorFader.setStatus(EGG::ColorFader::Status::Opaque);
        m_colorFader.fadeIn();

        OSResumeThread(&m_thread);
    }
}

} // namespace SP
