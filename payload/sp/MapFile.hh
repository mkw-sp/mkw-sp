#pragma once

#include <Common.hh>

#include <string_view>

namespace SP::MapFile {

bool IsLoaded();
void Load();
struct Symbol {
    u32 address;
    std::string_view name;
};
std::optional<Symbol> SymbolLowerBound(u32 address);
bool PrintAddressSymbolInfo(u32 address, char *symBuf, size_t symBufSize);

bool ScoreMatch(u32 symbol, u32 lr);

} // namespace SP::MapFile
