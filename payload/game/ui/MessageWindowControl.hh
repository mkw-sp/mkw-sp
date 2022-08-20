#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class MessageWindowControl : public LayoutUIControl {
public:
    MessageWindowControl();
    ~MessageWindowControl() override;

    void load(const char *dir, const char *file, const char *variant);
};
static_assert(sizeof(MessageWindowControl) == 0x174);

class MessageWindowControlScaleFade : public MessageWindowControl {
public:
    MessageWindowControlScaleFade();
    ~MessageWindowControlScaleFade() override;
};
static_assert(sizeof(MessageWindowControlScaleFade) == 0x174);

}  // namespace UI
