#pragma once

#include <optional>
#include <string_view>

#include <Common.hh>

namespace SP {

class IniReader {
public:
    struct Property {
        std::string_view section;
        std::string_view key;
        std::string_view value;
    };

    IniReader(std::string_view view);
    ~IniReader();
    std::optional<Property> next();

private:
    enum class State {
        None,
        WantSection,
        WantSectionEnd,
        InComment,
        WantDelim,
        WantValue,
        WantValueEnd,
        InCommentWantValue,
        PostWantValue,
    };

    bool wantsValueEnd() const;

    std::string_view m_view{};
    size_t m_pos = 0;
    u32 m_lineNum = 0;
    std::string_view m_section{};
    State m_state = State::None;
};

} // namespace SP
