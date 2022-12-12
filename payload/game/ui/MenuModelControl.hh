#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

enum class PageId;

class BackGroundModelControl : public LayoutUIControl {
public:
    void setModel(u32 model);

private:
    u8 _174[0x180 - 0x174];
};
static_assert(sizeof(BackGroundModelControl) == 0x180);

class DriverModelControl : public LayoutUIControl {
public:
    DriverModelControl();
    ~DriverModelControl() override;
    void initSelf() override;
    void calcSelf() override;
    void vf_20() override;
    void vf_28() override;
    void vf_2c() override;

    void load(const char *file, u32 localPlayerId);
    void onPageChange(PageId pageId);

private:
    u8 _174[0x188 - 0x174];
};
static_assert(sizeof(DriverModelControl) == 0x188);

class NoteModelControl : public LayoutUIControl {
public:
    REPLACE void beforePageAnim(PageId pageId);

private:
    u8 _174[0x17c - 0x174];
    bool m_hasPageAnim;
};
static_assert(sizeof(NoteModelControl) == 0x180);

} // namespace UI
