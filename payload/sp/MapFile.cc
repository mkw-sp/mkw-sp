#include "MapFile.hh"

#include <egg/core/eggSystem.hh>
extern "C" {
#include <revolution/os.h>
}
#include <sp/storage/DecompLoader.hh>
extern "C" {
#include <vendor/ff/ffconf.h>
}

#include <charconv>
#include <cmath>
#include <cstdio>
#include <ctype.h>
#include <optional>
#include <string_view>

namespace SP::MapFile {

#define SYMBOL_ADDRESS_LENGTH 10

struct Symbol {
    u32 address;
    std::string_view name;
};

static std::optional<std::string_view> s_mapFile = {};

bool IsLoaded() {
    return s_mapFile.has_value();
}

void Load() {
    if (IsLoaded()) {
        return;
    }

    char mapFilePath[FF_MAX_LFN + 1];
    snprintf(mapFilePath, sizeof(mapFilePath), "/bin/payload%c.SMAP.lzma", OSGetAppGamename()[3]);

    u8 *mapFile;
    size_t mapFileSize;
    auto *heap = EGG::TSystem::Instance().eggRootMEM2();
    if (!Storage::DecompLoader::LoadRO(mapFilePath, &mapFile, &mapFileSize, heap)) {
        SP_LOG("Failed to read the map file '%s'!", mapFilePath);
        return;
    }
    SP_LOG("Successfully read the map file '%s'!", mapFilePath);

    s_mapFile = std::string_view{reinterpret_cast<const char *>(mapFile), mapFileSize};
}

static std::optional<u32> GetSymbolAddress(std::string_view sv) {
    if (sv.size() < SYMBOL_ADDRESS_LENGTH) {
        return {};
    }
    sv = sv.substr(0, SYMBOL_ADDRESS_LENGTH);
    if (!sv.starts_with("0x")) {
        return {};
    }
    for (char c : sv.substr(2)) {
        if (!isxdigit(c)) {
            return {};
        }
    }

    u32 address;
    const std::from_chars_result result =
            std::from_chars(sv.data() + 2, sv.data() + SYMBOL_ADDRESS_LENGTH, address, 16);
    if (result.ec != std::errc{}) {
        return {};
    }

    return address;
}

static std::optional<Symbol> GetNextSymbol(u32 &mapFilePos) {
    if (mapFilePos == s_mapFile->size()) {
        return {};
    }

    std::string_view line = s_mapFile->substr(mapFilePos);

    size_t lineStartIndex = line.find_first_not_of('\n');
    if (lineStartIndex == std::string_view::npos) {
        lineStartIndex = line.size();
    }
    line = line.substr(lineStartIndex);
    mapFilePos += lineStartIndex;

    auto lineEndIndex = line.find('\n');
    if (lineEndIndex == std::string_view::npos) {
        lineEndIndex = line.size();
    }
    line = line.substr(0, lineEndIndex);
    mapFilePos += lineEndIndex;

    std::optional<u32> symbolAddress = GetSymbolAddress(line);
    if (!symbolAddress.has_value()) {
        return {};
    }
    line = line.substr(SYMBOL_ADDRESS_LENGTH);
    if (!line.starts_with(' ')) {
        return {};
    }
    line = line.substr(1);
    if (line.size() == 0) {
        return {};
    }

    return Symbol{*symbolAddress, line};
}

bool FindSymbol(u32 address, char *symbolNameBuffer, size_t symbolNameBufferSize) {
    if (!IsLoaded()) {
        return false;
    }

    u32 mapFilePos = 0;

    std::optional<Symbol> prevSymbol = GetNextSymbol(mapFilePos);
    if (!prevSymbol.has_value()) {
        return false;
    }

    while (true) {
        std::optional<Symbol> nextSymbol = GetNextSymbol(mapFilePos);
        if (!nextSymbol.has_value()) {
            break;
        }
        if (nextSymbol->address > address) {
            break;
        }

        prevSymbol = nextSymbol;
    }

    snprintf(symbolNameBuffer, symbolNameBufferSize, "%.*s [%c0x%08X]", prevSymbol->name.size(),
            prevSymbol->name.data(), address < prevSymbol->address ? '-' : '+',
            std::abs(static_cast<int>(address - prevSymbol->address)));
    return true;
}

} // namespace SP::MapFile
