#include "MapFile.hh"

extern "C" {
#include "StackTrace.h"
}

#include <egg/core/eggSystem.hh>
extern "C" {
#include <revolution/os.h>
}
#include <sp/storage/DecompLoader.hh>
extern "C" {
#include <vendor/ff/ffconf.h>
}

#include <vendor/magic_enum/magic_enum.hpp>

#include <charconv>
#include <cmath>
#include <cstdio>
#include <ctype.h>
#include <optional>
#include <string_view>

namespace SP::MapFile {

#define SYMBOL_ADDRESS_LENGTH 10

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

std::optional<Symbol> SymbolLowerBound(u32 address) {
    if (!IsLoaded()) {
        return std::nullopt;
    }

    u32 mapFilePos = 0;

    std::optional<Symbol> prevSymbol = GetNextSymbol(mapFilePos);
    if (!prevSymbol.has_value()) {
        return std::nullopt;
    }

    while (true) {
        std::optional<Symbol> nextSymbol = GetNextSymbol(mapFilePos);
        if (!nextSymbol.has_value()) {
            break;
        }
        if (nextSymbol->address >= address) {
            break;
        }

        prevSymbol = nextSymbol;
    }

    return prevSymbol;
}

bool PrintAddressSymbolInfo(u32 address, char *symBuf, size_t symBufSize) {
    auto it = SymbolLowerBound(address);
    if (it && ScoreMatch(it->address, address)) {
        assert(address >= it->address);
        return 0 < snprintf(symBuf, symBufSize, "%.*s [+0x%08X]", it->name.size(), it->name.data(),
                           address - it->address);
    }
    auto bin = magic_enum::enum_name(ClassifyPointer((void *)address));
    void *ported = PortPointer((void *)address);
    if (ported == (void *)address) {
        return 0 < snprintf(symBuf, symBufSize, "<Symbol not found: %.*s>", bin.size(), bin.data());
    }
    return 0 < snprintf(symBuf, symBufSize, "<Symbol not found: %.*s (%p vanilla PAL)>", bin.size(),
                       bin.data(), ported);
}

bool ScoreMatch(u32 symbol, u32 lr) {
    if (lr < symbol) {
        return false;
    }
    switch (ClassifyPointer((void *)lr)) {
    case BINARY_SP:
        // Assume SP symbols are contiguous
        return true;
    case BINARY_DOL:
    case BINARY_REL:
        break;
    case BINARY_ILLEGAL:
        return false;
    }
    // Largest function in game
    if (lr - symbol > 0x4D14) {
        return false;
    }
    for (u32 it = symbol; it < lr; ++it) {
        u32 ins = *reinterpret_cast<volatile u32 *>(it);
        if (ins == 0x4E800020) { // blr
            return false;
        }
    }
    return true;
}

} // namespace SP::MapFile
