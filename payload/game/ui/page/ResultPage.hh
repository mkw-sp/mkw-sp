#pragma once

#include "game/ui/Page.hh"

namespace UI {

class ResultPage : public Page {
public:
    ResultPage();
    ~ResultPage() override;
    PageId getReplacement() override = 0;
    void onInit() override = 0;
    void onActivate() override;
    void afterCalc() override;
    TypeInfo *getTypeInfo() const override;
    virtual bool isBusy() = 0;
    virtual void vf_68() = 0;

private:
    u8 _044[0x1a0 - 0x044];
};
static_assert(sizeof(ResultPage) == 0x1a0);

} // namespace UI
