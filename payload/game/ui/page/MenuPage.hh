#pragma once

#include "game/ui/Button.hh"
#include "game/ui/Page.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

namespace UI {

class MenuPage : public Page {
public:
    virtual void vf_64();
    virtual void vf_68();
    virtual void vf_6c();
    virtual void vf_70();
    virtual void vf_74();
    virtual void selectDefault(PushButton *button);
    virtual void vf_7c();
    virtual void vf_80();
    virtual void vf_84();
    virtual void vf_88();
    virtual void vf_8c();
    virtual void vf_90();
    virtual void vf_94();

    static TypeInfo *GetTypeInfo();

protected:
    void startReplace(PageId pageId, const PushButton *button);
    void onBackButtonFront(const PushButton *button);
    void requestChangeSection(SectionId sectionId, const PushButton *button);
    void pushMessage(u32 messageId, MessageInfo *info = nullptr);
    bool checkAllMulti();

protected:
    MiiGroup *m_miiGroup;
    PushButton **m_buttons;

private:
    u8 _04c[0x2bc - 0x04c];

protected:
    CtrlMenuPageTitleText *m_instructionText;

private:
    u8 _2c0[0x3e8 - 0x2c0];

protected:
    PageId m_replacement;

public:
    PageId m_prevId;

private:
    u8 _3f0[0x430 - 0x3f0];
};
static_assert(sizeof(MenuPage) == 0x430);

} // namespace UI
