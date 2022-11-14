#pragma once

#include <Common.hh>

namespace Registry {

extern const char *courseFilenames[0x28];

u32 GetCharacterMessageId(u32 characterId, bool resolveMiiNames);

const char *GetCharacterPane(u32 characterId);

const char *REPLACED(getItemPane)(u32 itemId, u32 count);
REPLACE const char *getItemPane(u32 itemId, u32 count);

} // namespace Registry