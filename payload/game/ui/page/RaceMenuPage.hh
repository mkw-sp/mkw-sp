#pragma once

#include "game/ui/Page.hh"

namespace UI {

class RaceMenuPage : public Page {
public:
    virtual void setReplacement(PageId id);
    virtual void vf_68();
    virtual void vf_6c();
    virtual void vf_70();
    virtual void vf_74();
    virtual void vf_78();

    static TypeInfo *GetTypeInfo();

private:
    u8 _044[0x344 - 0x044];
};
static_assert(sizeof(RaceMenuPage) == 0x344);

} // namespace UI
