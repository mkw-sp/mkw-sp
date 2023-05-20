#pragma once

#include <Common.hh>

#include <optional>
#include <string_view>

namespace SP::MapFile {

bool IsLoaded();
void Load();
struct Symbol {
    u32 address;
    std::string_view name;
};
std::optional<Symbol> SymbolLowerBound(u32 address);
bool FindSymbol(u32 address, char *symbolNameBuffer, size_t symbolNameBufferSize);

bool ScoreMatch(u32 symbol, u32 lr);

} // namespace SP::MapFile
