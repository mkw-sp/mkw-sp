#pragma once

#include "game/ui/Page.hh"

extern "C" {
#include "game/system/TextWriter.h"
}

#include <string_view>

namespace UI {

/// Wrapper for Console.h's C TextWriter
class SPTextWriter : TextWriter {
public:
    void configure(Rect box, void *res) {
        TextWriter_configure(this, box, res);
    }
    void reset() {
        TextWriter_reset(this);
    }
    bool beginDraw() {
        TextWriter_beginDraw(this);
        return true;
    }
    void draw(std::string_view str, float size, u8 alpha_override) {
        TextWriter_draw(this, str.data(), str.size(), size, alpha_override);
    }
    void endDraw() {
        TextWriter_endDraw(this);
    }
};

class CourseDebugPage : public Page {
public:
    CourseDebugPage() = default;
    CourseDebugPage(const CourseDebugPage &) = delete;
    CourseDebugPage(CourseDebugPage &&) = delete;

    void onInit() override;
    PageId getReplacement() override;
    void onActivate() override;
    void onDeactivate() override;

private:
    class Control : public LayoutUIControl {
    public:
        void load();

    private:
        void draw(int pass) override;
        SPTextWriter m_textWriter;
    };

    void onKeyPress(char key, u8 mods);
    static void keypressCb(char key, u8 mods, void *user);

    void onFront(u32 localPlayerId);
    void onBack(u32 localPlayerId);
    void onUp(u32 localPlayerId);
    void onDown(u32 localPlayerId);
    void onLeft(u32 localPlayerId);
    void onRight(u32 localPlayerId);

    PageInputManager m_inputManager;
    Control m_text;
    PageId m_replacement = PageId::None;
    MenuInputManager::Handler<CourseDebugPage> m_onFront{this, &CourseDebugPage::onFront};
    MenuInputManager::Handler<CourseDebugPage> m_onBack{this, &CourseDebugPage::onBack};
    MenuInputManager::Handler<CourseDebugPage> m_onUp{this, &CourseDebugPage::onUp};
    MenuInputManager::Handler<CourseDebugPage> m_onDown{this, &CourseDebugPage::onDown};
    MenuInputManager::Handler<CourseDebugPage> m_onLeft{this, &CourseDebugPage::onLeft};
    MenuInputManager::Handler<CourseDebugPage> m_onRight{this, &CourseDebugPage::onRight};
};

} // namespace UI
