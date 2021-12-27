#pragma once

#include "GlobalContext.h"
#include "RegisteredPadManager.h"
#include "Section.h"

typedef struct {
    Section *currentSection;
    u8 _04[0x34 - 0x04];
    RegisteredPadManager registeredPadManager;
    u8 _90[0x98 - 0x90];
    GlobalContext *globalContext;
} SectionManager;

extern SectionManager *s_sectionManager;

void SectionManager_init(SectionManager *this);
