#pragma once

#include "dvd.h"

void DVDExInit(void);

BOOL DVDExOpen(const char *fileName, DVDFileInfo *fileInfo);

const char *DVDExConvertEntrynumToPath(s32 entrynum);
