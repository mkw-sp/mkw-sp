#include "GlobalSettings.hh"

#include "sp/storage/Storage.hh"

#include <cstring>

namespace SP::GlobalSettings {

const char name[] = "Global Settings";

const u32 categoryMessageIds[] = { 10378 };

// clang-format off
const Entry entries[] = {
    [static_cast<u32>(Setting::FileReplacement)] = {
        .category = Category::MyStuff,
        .name = magic_enum::enum_name(Setting::FileReplacement),
        .messageId = 10379,
        .defaultValue = static_cast<u32>(FileReplacement::All),
        .valueCount = magic_enum::enum_count<FileReplacement>(),
        .valueNames = magic_enum::enum_names<FileReplacement>().data(),
        .valueMessageIds = (u32[]) { 10380, 10381, 10382 },
        .valueExplanationMessageIds = (u32[]) { 10383, 10384, 10385 },
    },
    [static_cast<u32>(Setting::BootSection)] = {
        .category = Category::UI,
        .name = magic_enum::enum_name(Setting::BootSection),
        .messageId = 0,
        .defaultValue = 0xFFFFFFFF,
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
    [static_cast<u32>(Setting::LogFileRetention)] = {
        .category = Category::Miscellaneous,
        .name = magic_enum::enum_name(Setting::LogFileRetention),
        .messageId = 0,
        .defaultValue = 7,
        .valueCount = 0,
        .valueNames = nullptr,
        .valueMessageIds = nullptr,
        .valueExplanationMessageIds = nullptr,
    },
};
// clang-format on

static Settings instance{};

Settings &Instance() {
    return instance;
}

void Init() {
    instance.reset();

    const wchar_t *path = L"/mkw-sp/settings.ini";

    // TODO: Hopefully this is enough. Can always stream the file if not.
    char iniBuffer[2048];
    auto size = Storage::ReadFile(path, iniBuffer, sizeof(iniBuffer));
    if (size) {
        instance.readIni(iniBuffer, *size);
    }

    instance.writeIni(iniBuffer, sizeof(iniBuffer));
    SP::Storage::WriteFile(path, iniBuffer, strlen(iniBuffer), true);
}

} // namespace SP::GlobalSettings
