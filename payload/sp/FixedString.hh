#pragma once

extern "C" {
#include "sp/WideUtil.h"

#include <Common.h>
}

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

namespace SP {

template <size_t N>
struct FixedString {
    constexpr FixedString() = default;
    constexpr FixedString(const FixedString &) = default;
    constexpr ~FixedString() = default;
    constexpr FixedString(std::string_view view) {
        set(view);
    }
    constexpr FixedString(const char *cstr) {
        set(cstr);
    }
    constexpr operator std::string_view() const {
        return std::string_view(m_buf.data(), m_len);
    }

    constexpr void set(std::string_view view) {
        m_len = std::min(N - 1, view.size());
        std::copy_n(view.data(), m_len, m_buf.data());
        m_buf[m_len] = '\0';
    }

    constexpr bool operator==(const FixedString &) const = default;
    constexpr bool operator!=(const FixedString &) const = default;

    const char *c_str() const {
        // Always null terminated
        return m_buf.data();
    }

    size_t m_len = 0;
    // Null terminated, just in case
    std::array<char, N> m_buf = {};
};

template <size_t N>
struct WFixedString {
    constexpr WFixedString() = default;
    constexpr WFixedString(const WFixedString &) = default;
    constexpr ~WFixedString() = default;
    constexpr WFixedString(std::string_view view) {
        if (view.size() >= m_buf.size()) {
            view = view.substr(0, N - 1);
        }

        auto written = Util_toUtf16(m_buf.data(), m_buf.size(), view.data(), view.size());
        m_buf[written] = L'\0';
    }

    const wchar_t *c_str() const {
        // Always null terminated
        return m_buf.data();
    }

    size_t m_len = 0;
    std::array<wchar_t, N> m_buf = {};
};

} // namespace SP
