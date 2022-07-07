#include "IniReader.hh"

extern "C" {
#include <revolution.h>
}

namespace SP {

IniReader::IniReader(std::string_view view) : m_view(view) {}

IniReader::~IniReader() = default;

std::optional<IniReader::Property> IniReader::next() {
    auto lineStart = m_view.begin() + m_pos;
    decltype(lineStart) token{};
    decltype(lineStart) lastNonSpace{};

    std::string_view key{};

    decltype(lineStart) it;
    for (it = lineStart; it != m_view.end(); ++it, ++m_pos) {
        if (m_state == State::PostWantValue) {
            ++m_lineNum;
            m_state = State::None;
        }
        switch (*it) {
        case ' ':
            if (m_state == State::None || m_state == State::WantDelim ||
                    m_state == State::WantValue) {
                break;
            }
            // Treat as identifier
            goto Identifier;
        case '[':
            if (m_state == State::None) {
                token = it;
                lastNonSpace = token;
                m_state = State::WantSection;
                break;
            }
            // Treat as identifier
            goto Identifier;
        case ']':
            if (m_state == State::WantSectionEnd ||
                    m_state == State::WantSection /* Empty section tag */) {
                m_section = std::string_view(token, m_state == State::WantSection ? token : it);
                m_state = State::None;
                break;
            }
            // Treat as identifier
            goto Identifier;
        case '\n':
            if (m_state == State::None || m_state == State::InComment) {
                m_state = State::None;
                ++m_lineNum;
                lineStart = it + 1;
                break;
            }
            if (wantsValueEnd()) {
                ++m_pos;
            StateWantValue:
                m_state = State::PostWantValue;
                return Property {
                    m_section,
                    key,
                    std::string_view(token, m_state == State::WantValue ? token : lastNonSpace + 1),
                };
            }
            switch (m_state) {
            case State::WantSection:
            case State::WantSectionEnd:
                SP_LOG("<%i:%i> Expecting ']', instead saw end of line", m_lineNum, it - lineStart);
                return {};
            case State::WantDelim:
                SP_LOG("<%i:%i> Expecting '=' or ':', instead saw end of line", m_lineNum,
                        it - lineStart);
                return {};
            default:
                __builtin_unreachable();
            }
        case '#':
        case ';':
            if (m_state == State::None || m_state == State::InComment) {
                m_state = State::InComment;
                break;
            }
            if (wantsValueEnd()) {
                m_state = State::InCommentWantValue;
                break;
            }
            switch (m_state) {
            case State::WantSection:
            case State::WantSectionEnd:
                SP_LOG("<%i:%i> Expecting ']', instead saw comment", m_lineNum, it - lineStart);
                return {};
            case State::WantDelim:
                SP_LOG("<%i:%i> Expecting '=' or ':', instead saw comment", m_lineNum,
                        it - lineStart);
                return {};
            default:
                __builtin_unreachable();
            }
        case '=':
        case ':':
            if (m_state == State::WantDelim) {
                m_state = State::WantValue;
                key = std::string_view(token, lastNonSpace + 1);
                break;
            }
            // Treat as identifier
            goto Identifier;
        default:
            if (m_state == State::None) {
                m_state = State::WantDelim;
                token = it;
                lastNonSpace = token;
                break;
            }
            // Treat as identifier
        Identifier:
            switch (m_state) {
            case State::InComment:
            case State::InCommentWantValue:
                continue;
            case State::WantSection:
                token = it;
                lastNonSpace = token;
                m_state = State::WantSectionEnd;
                continue;
            case State::WantValue:
                token = it;
                lastNonSpace = token;
                m_state = State::WantValueEnd;
                continue;
            case State::WantSectionEnd:
            case State::WantDelim:
            case State::WantValueEnd:
                if (*it != ' ') {
                    lastNonSpace = it;
                }
                continue;
            case State::None:
            case State::PostWantValue:
                __builtin_unreachable();
            }

            __builtin_unreachable();
        }
    }
    switch (m_state) {
    case State::None:
    case State::InComment:
        return {};
    case State::WantSection:
    case State::WantSectionEnd:
        SP_LOG("<%i:%i> Expecting ']', instead saw EOF", m_lineNum, m_view.end() - lineStart);
        return {};
    case State::WantDelim:
        SP_LOG("<%i:%i> Expecting '=' or ':', instead saw EOF", m_lineNum,
                m_view.end() - lineStart);
        return {};
    case State::WantValue:
    case State::WantValueEnd:
    case State::InCommentWantValue:
        goto StateWantValue;
    case State::PostWantValue:
        return {};
    default:
        __builtin_unreachable();
    }
}

bool IniReader::wantsValueEnd() const {
    switch (m_state) {
    case State::WantValueEnd:
    case State::WantValue: // Empty value
    case State::InCommentWantValue:
        return true;
    default:
        return false;
    }
}

} // namespace SP
