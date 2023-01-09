#include "GlobalSettings.hh"

#include "sp/storage/Storage.hh"

#include <cstring>

namespace SP::GlobalSettings {

const char name[] = "Global Settings";

const u32 categoryMessageIds[] = { 0 };

const Entry entries[] = {
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
