#include "CourseDebugPage.hh"

#include "game/sound/SoundId.hh"
#include "game/system/RaceConfig.hh"
#include "game/util/Registry.hh"

#include <sp/storage/DirectoryRange.hh>
extern "C" {
#include <sp/Commands.h>
#include <sp/keyboard/Keyboard.h>
}

#include <cwchar>

#include <vendor/magic_enum/magic_enum.hpp>

namespace UI {

/// Read .szs files from the "My Stuff" directory.
static auto ScanMyStuff() {
    auto dir = SP::Storage::OpenDir(L"/mkw-sp/My Stuff");
    assert(dir && "Failed to open the \"/mkw-sp/My Stuff\" directory");
    auto is_file = [&](auto &x) { return x.type == SP::Storage::NodeType::File; };
    return SP::Storage::DirEntryRange{std::move(*dir)} | std::ranges::views::filter(is_file);
}

/// Updates the selected course path.
static void SelectTrackForLoad(const SP::Storage::NodeInfo &info) {
    auto *rc = System::RaceConfig::Instance();
    assert(rc);
    std::wstring_view name = info.name;
    char buf[128];
    snprintf(buf, sizeof(buf), "/mkw-sp/My Stuff/%.*ls", name.size(), name.data());
    rc->m_spMenu.pathReplacement.set(buf);
}

/// Gets the selected course path.
static const char *GetSelectedTrack() {
    auto *rc = System::RaceConfig::Instance();
    return rc->m_spMenu.pathReplacement.c_str();
}

/// Updates the course parameters.
static void SetTrackConfig(Registry::Course slot, Registry::CourseBgm music) {
    auto &menuScenario = System::RaceConfig::Instance()->menuScenario();
    menuScenario.courseId = slot;
    System::RaceConfig::Instance()->m_spMenu.musicReplacement = static_cast<Sound::SoundId>(music);
}

/// Print the file name and its size.
static int PrintFileInfo(std::span<char> buf, const SP::Storage::NodeInfo &info, bool isSelected) {
    OSCalendarTime time;
    OSTicksToCalendarTime(info.tick, &time);
    return snprintf(buf.data(), buf.size(), "[%s] %d/%d/%d &4%ls&f %llu bytes\n",
            isSelected ? "&aX&f" : " ", time.mon, time.mday, time.year, info.name, info.size);
}

/// Print the entire track list
static int PrintTrackList(std::span<char> buf, int selectedIdx) {
    const int linesPerPage = 30;
    int startIdx = std::max(0, selectedIdx - linesPerPage / 2);

    int ofs = 0;
    int i = 0;
    for (auto info : ScanMyStuff()                       //
                    | std::ranges::views::drop(startIdx) //
                    | std::ranges::views::take(linesPerPage)) {
        bool isSelected = selectedIdx == startIdx + i++;
        ofs += PrintFileInfo(buf.subspan(ofs), info, isSelected);
    }

    return ofs;
}

/// Apply the selected track index
static void ApplyTrackListSelection(int selectedIdx) {
    int i = 0;
    for (auto info : ScanMyStuff()) {
        if (selectedIdx == i) {
            SelectTrackForLoad(info);
            break;
        }
        ++i;
    }
}

namespace {

/// Text-based enum manipulator using magic_enum
template <typename T>
class TextSlider {
public:
    void add(int displacement) {
        m_idx = (Max + (m_idx + displacement)) % Max;
    }
    T get() {
        return magic_enum::enum_value<T>(m_idx);
    }
    int print(std::span<char> buf, const char *label, bool selected) {
        auto e = get();
        return snprintf(buf.data(), buf.size(), "%s%s&f: &4%s&f [%d/%d]\n", selected ? "&a" : "&f",
                label, magic_enum::enum_name(e).data(), m_idx, Max - 1);
    }

private:
    static constexpr int Max = magic_enum::enum_count<T>();
    int m_idx = 0;
};

/// Base class for a text-based subpage
struct IState {
    virtual void print(std::span<char> /* buf */) {}
    virtual void onKeyPress(char /* c */) {}
};

/// Text-based state for selecting a track
class SelectTrackState final : public IState {
public:
    void print(std::span<char> buf) override {
        int it = snprintf(buf.data(), buf.size(), "&fSelect a track to load:\n");
        PrintTrackList(buf.subspan(it), m_selectedTrackIdx);
        ApplyTrackListSelection(m_selectedTrackIdx);
    }

    void onKeyPress(char c) override {
        switch (c) {
        case kSimpleEvent_ArrowU:
            --m_selectedTrackIdx;
            break;
        case kSimpleEvent_ArrowD:
            ++m_selectedTrackIdx;
            break;
        }
    }

private:
    int m_selectedTrackIdx = 0;
};

/// Text-based state for selecting a slot/BGM
class SelectInfoState final : public IState {
public:
    void print(std::span<char> buf) override {
        int x = snprintf(buf.data(), buf.size(), "&fLoading %s:\n", GetSelectedTrack());
        x += m_courseSlot.print(buf.subspan(x), "Slot", m_selectedSliderIdx == 0);
        m_musicSlot.print(buf.subspan(x), "BGM", m_selectedSliderIdx == 1);
        SetTrackConfig(m_courseSlot.get(), m_musicSlot.get());
    }

    void onKeyPress(char c) override {
        switch (c) {
        case kSimpleEvent_ArrowL:
            sliderInput(-1);
            break;
        case kSimpleEvent_ArrowR:
            sliderInput(1);
            break;
        case kSimpleEvent_ArrowD:
        case kSimpleEvent_ArrowU:
            m_selectedSliderIdx = !m_selectedSliderIdx;
            break;
        }
    }

private:
    void sliderInput(int dir) {
        if (!m_selectedSliderIdx) {
            m_courseSlot.add(dir);
        } else {
            m_musicSlot.add(dir);
        }
    }

    TextSlider<Registry::Course> m_courseSlot;
    TextSlider<Registry::CourseBgm> m_musicSlot;
    int m_selectedSliderIdx = 0;
};

/// Text-based track selector
class TextTrackSelector {
public:
    void reset() {
        new (this) TextTrackSelector;
    }

    void refresh() {
        state().print(m_textBuffer);
    }

    bool transition(int dir) {
        bool edge = dir > 0 ? m_index == 1 : m_index == 0;
        m_index = std::max(std::min(m_index + dir, 1), 0);
        refresh();
        return edge;
    }

    void onKeyPress(char c) {
        state().onKeyPress(c);
        refresh();
    }

    std::array<char, 4096 * 4> m_textBuffer{};

private:
    SelectTrackState m_track;
    SelectInfoState m_info;
    int m_index = 0;

    IState &state() {
        return m_index == 0 ? static_cast<IState &>(m_track) : static_cast<IState &>(m_info);
    }
};

} // namespace

static TextTrackSelector s_trackSelector;

void CourseDebugPage::Control::load() {
    const char *groups[] = {"Group_00", "A0", nullptr, nullptr};
    LayoutUIControl::load("control", "CourseDebug", "CourseDebug", groups);
}
void CourseDebugPage::Control::draw(int pass) {
    LayoutUIControl::draw(pass);
    if (pass == 0) {
        Rect box = {.top = 70.0f, .bottom = 800.0f, .left = 10.0f, .right = 900.0f};
        m_textWriter.configure(box, nullptr);
        m_textWriter.reset();
        if (m_textWriter.beginDraw()) {
            const char *buf = s_trackSelector.m_textBuffer.data();
            m_textWriter.draw(buf, 10, 255);
            m_textWriter.endDraw();
        }
    }
}

void CourseDebugPage::onFront(u32 /* localPlayerId */) {
    if (s_trackSelector.transition(1)) {
        m_replacement = PageId::TimeAttackTop;
        startReplace(Anim::None, 0.0f);
    }
}
void CourseDebugPage::onBack(u32 /* localPlayerId */) {
    if (s_trackSelector.transition(-1)) {
        m_replacement = PageId::DriftSelect;
        startReplace(Anim::None, 0.0f);
    }
}
void CourseDebugPage::onUp(u32 /* localPlayerId */) {
    s_trackSelector.onKeyPress(kSimpleEvent_ArrowU);
}
void CourseDebugPage::onDown(u32 /* localPlayerId */) {
    s_trackSelector.onKeyPress(kSimpleEvent_ArrowD);
}
void CourseDebugPage::onLeft(u32 /* localPlayerId */) {
    s_trackSelector.onKeyPress(kSimpleEvent_ArrowL);
}
void CourseDebugPage::onRight(u32 /* localPlayerId */) {
    s_trackSelector.onKeyPress(kSimpleEvent_ArrowR);
}

void CourseDebugPage::onKeyPress(char c, u8 mods) {
    // Since the game's inputs do not allow rapid pressing, enable CTRL + DOWN on keyboard for
    // faster navigation if available.
    if (mods & kSimpleMods_CTRL) {
        s_trackSelector.onKeyPress(c);
    }
}

void CourseDebugPage::onInit() {
    m_inputManager.init(1, false);
    setInputManager(&m_inputManager);

    {
        using enum MenuInputManager::InputId;
        m_inputManager.setHandler(Front, &m_onFront, false);
        m_inputManager.setHandler(Back, &m_onBack, false);
        m_inputManager.setHandler(Up, &m_onUp, true);
        m_inputManager.setHandler(Down, &m_onDown, true);
        m_inputManager.setHandler(Left, &m_onLeft, true);
        m_inputManager.setHandler(Right, &m_onRight, true);
    }

    initChildren(1);
    insertChild(0, &m_text, 0);
    m_text.load();
}

void CourseDebugPage::onActivate() {
    s_trackSelector.reset();
    s_trackSelector.refresh();
    m_text.m_animator.setAnimation(0, 0, 0.0f);
    SP_SetKeypressCallback(keypressCb, this);
}

void CourseDebugPage::keypressCb(char key, u8 mods, void *user) {
    auto *page = reinterpret_cast<CourseDebugPage *>(user);
    page->onKeyPress(key, mods);
}

void CourseDebugPage::onDeactivate() {
    SP_SetKeypressCallback(nullptr, nullptr);
}

PageId CourseDebugPage::getReplacement() {
    return m_replacement;
}

} // namespace UI

sp_define_command("/cd", "Course debug list", const char *) {
    UI::s_trackSelector.refresh();
    SP_LOG("%s", UI::s_trackSelector.m_textBuffer.data());
}
