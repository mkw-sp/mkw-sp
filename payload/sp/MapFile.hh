#pragma once

#include <Common.hh>

namespace SP::MapFile {

bool IsLoaded();
void Load();
bool FindSymbol(u32 address, char *symbolNameBuffer, size_t symbolNameBufferSize);

} // namespace SP::MapFile
