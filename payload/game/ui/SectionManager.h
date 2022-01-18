#pragma once

#include "GlobalContext.h"
#include "RegisteredPadManager.h"
#include "SaveManagerProxy.h"
#include "Section.h"

typedef struct {
    Section *currentSection;
    u8 _04[0x34 - 0x04];
    RegisteredPadManager registeredPadManager;
    SaveManagerProxy *saveManagerProxy;
    u8 _94[0x98 - 0x94];
    GlobalContext *globalContext;
} SectionManager;

extern SectionManager *s_sectionManager;

void SectionManager_init(SectionManager *this);

void SectionManager_setNextSection(SectionManager *this, u32 sectionId, u32 animation);

void SectionManager_startChangeSection(SectionManager *this, u32 delay, u32 color);
