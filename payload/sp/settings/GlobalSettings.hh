#pragma once

#include "sp/settings/Settings.hh"

namespace SP::GlobalSettings {

enum class Setting {
    FileReplacement,
    BootSection,
    LogFileRetention,
};

enum class Category {
    MyStuff,
    UI,
    Miscellaneous,
};

enum class FileReplacement {
    Off,
    BRSTMsOnly,
    All,
};

typedef Settings::Group<Category> Group;
typedef Settings::Entry<Category> Entry;
template <Setting S>
using Helper = Settings::Helper<Setting, S>;

extern const char name[];
constexpr auto categoryNames = magic_enum::enum_names<Category>();
extern const u32 categoryMessageIds[];
constexpr u32 entryCount = magic_enum::enum_count<Setting>();
extern const Entry entries[];
constexpr Group group{name, categoryNames.data(), categoryMessageIds, entryCount, entries};

typedef Settings::Settings<Category, GlobalSettings::group> Settings;

//
// Workaround for [this](https://bugs.llvm.org/show_bug.cgi?id=36284) Clang bug.
// Although initially reported for <= version 8, it still appears to exist on the latest version
// (Clang 16.0.0)
//
// https://godbolt.org/z/KohzEafxM
//
// Since Settings is initialized by `Init`, this is not too difficult to workaround...
//
extern char wtf[];
inline Settings &Instance() {
    return reinterpret_cast<Settings &>(wtf);
}
void Init();

template <Setting S>
Helper<S>::type Get() {
    return Instance().get<Setting, S>();
}

} // namespace SP::GlobalSettings

namespace SP::Settings {

template <>
struct Helper<GlobalSettings::Setting, GlobalSettings::Setting::FileReplacement> {
    using type = SP::GlobalSettings::FileReplacement;
};

template <>
struct Helper<GlobalSettings::Setting, GlobalSettings::Setting::BootSection> {
    using type = u32;
};

template <>
struct Helper<GlobalSettings::Setting, GlobalSettings::Setting::LogFileRetention> {
    using type = u32;
};

} // namespace SP::Settings
