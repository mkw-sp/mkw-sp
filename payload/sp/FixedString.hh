#pragma once

#include <Common.hh>

#include <algorithm>
#include <string_view>

extern "C" {
#include "sp/WideUtil.h"
}

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

    void setUTF16(std::wstring_view view) {
        m_len = Util_toUtf8(m_buf.data(), m_buf.size() - 1, view.data(), view.size());
        m_buf[m_len] = '\0';
    }

    static FixedString<N> fromUTF16(std::wstring_view view) {
        FixedString<N> result;
        result.setUTF16(view);
        return result;
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
    constexpr WFixedString(std::wstring_view view) {
        set(view);
    }
    constexpr WFixedString(const wchar_t *cstr) {
        set(cstr);
    }
    constexpr operator std::wstring_view() const {
        return std::wstring_view(m_buf.data(), m_len);
    }

    constexpr void set(std::wstring_view view) {
        m_len = std::min(N - 1, view.size());
        std::copy_n(view.data(), m_len, m_buf.data());
        m_buf[m_len] = L'\0';
    }

    constexpr bool operator==(const WFixedString &) const = default;
    constexpr bool operator!=(const WFixedString &) const = default;

    const wchar_t *c_str() const {
        // Always null terminated
        return m_buf.data();
    }

    void setUTF8(std::string_view view) {
        m_len = Util_toUtf16(m_buf.data(), m_buf.size() - 1, view.data(), view.size());
        m_buf[m_len] = L'\0';
    }

    static WFixedString<N> fromUTF8(std::string_view view) {
        WFixedString<N> result;
        result.setUTF8(view);
        return result;
    }

    size_t m_len = 0;
    // Null terminated, just in case
    std::array<wchar_t, N> m_buf = {};
};

} // namespace SP
