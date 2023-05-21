#pragma once

#include "egg/core/eggDisposer.hh"

namespace EGG {

class EffectResource : public Disposer {
public:
    EffectResource(void *breff, void *breft);
    ~EffectResource() override;
    virtual void setResource(void *breff, void *breft);
    virtual void resetResource();

private:
    void *m_breff;
    void *m_breft;
    u8 _18[0x20 - 0x18];

    static nw4r::ut::List s_list;
};
static_assert(sizeof(EffectResource) == 0x20);

} // namespace EGG
