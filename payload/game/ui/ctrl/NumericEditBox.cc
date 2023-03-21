#include "NumericEditBox.hh"

namespace UI {

void NumericEditBox::setNumber(u64 number) {
    for (u32 i = m_digitCount; i-- > 0;) {
        m_letters[i].m_value = number % 10;
        MessageInfo info{};
        info.intVals[0] = number % 10;
        m_letters[i].setMessageAll(5105, &info);
        number /= 10;
        m_letters[i].setPaneVisible("cursor_n", false);
    }
    m_position = m_digitCount;
    m_letters[m_position].setPaneVisible("cursor_n", true);
}

} // namespace UI
