#pragma once

#include <Common.hh>

namespace Registry {

u32 GetCharacterMessageId(u32 characterId, bool resolveMiiNames);

const char *GetCharacterPane(u32 characterId);

} // namespace Registry
