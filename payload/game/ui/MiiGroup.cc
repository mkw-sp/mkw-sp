#include "MiiGroup.hh"

#include <iterator>

namespace UI {

MiiGroup::~MiiGroup() {
    dt(-1);
}

void MiiGroup::swap(u32 i0, u32 i1) {
    System::Mii *tmp = m_miis[i0];
    m_miis[i0] = m_miis[i1];
    m_miis[i1] = tmp;
    for (u32 i = 0; i < std::size(m_textures); i++) {
        if (m_textures[i] != nullptr) {
            Texture tmp = m_textures[i][i0];
            m_textures[i][i0] = m_textures[i][i1];
            m_textures[i][i1] = tmp;
        }
    }
}

} // namespace UI
