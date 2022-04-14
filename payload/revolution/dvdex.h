#pragma once

#include "dvd.h"

void DVDExInit(void);

BOOL DVDExOpen(const char *fileName, DVDFileInfo *fileInfo);

BOOL DVDExClose(DVDFileInfo *fileInfo);

s32 DVDExConvertPathToEntrynum(const char *fileName);
