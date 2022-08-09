#include "Mii.hh"

namespace System {

const wchar_t *Mii::name() const {
    return m_name;
}

const MiiId *Mii::id() const {
    return &m_id;
}

} // namespace System
