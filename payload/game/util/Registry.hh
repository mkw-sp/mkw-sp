#pragma once

#include <Common.hh>

namespace Registry {

extern const char *courseFilenames[0x28];

u32 GetCharacterMessageId(u32 characterId, bool resolveMiiNames);
REPLACE u32 isBattle();

const char *GetCharacterPane(u32 characterId);

const char *REPLACED(GetItemPane)(u32 itemId, u32 count);
REPLACE const char *GetItemPane(u32 itemId, u32 count);

} // namespace Registry
