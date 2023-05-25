#include "eggEffectResource.hh"

namespace EGG {

EffectResource::EffectResource(void *breff, void *breft) : m_breff(nullptr), m_breft(nullptr) {
    if (breff && breft) {
        setResource(breff, breft);
    }
    s_list.append(this);
}

EffectResource::~EffectResource() {
    if (m_breff && m_breft) {
        resetResource();
    }
    s_list.remove(this);
}

} // namespace EGG
