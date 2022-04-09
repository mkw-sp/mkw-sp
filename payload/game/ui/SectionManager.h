#pragma once

#include "GlobalContext.h"
#include "RegisteredPadManager.h"
#include "SaveManagerProxy.h"
#include "Section.h"

typedef enum SectionChangeState {
    IDLE = 0x0,
    REINIT_REQUESTED = 0x1,
    CHANGE_REQUESTED = 0x2,
    REINIT_READY = 0x3,
    CHANGE_READY = 0x4,
    OVERRIDE_READY = 0x5,
} SectionChangeState;
static_assert(sizeof(SectionChangeState) == 0x4);

typedef struct {
    Section *currentSection;
    u8 _04[0x0c - 0x4];
    u32 nextSectionId;
    u8 _10[0x1c - 0x10];
    s32 changeTimer;
    u8 _20[0x28 - 0x20];
    u32 fadeColor;
    u8 _2c[0x30 - 0x2c];
    SectionChangeState state;
    RegisteredPadManager registeredPadManager;
    SaveManagerProxy *saveManagerProxy;
    u8 _94[0x98 - 0x94];
    GlobalContext *globalContext;
} SectionManager;
static_assert(sizeof(SectionManager) == 0x9c);

extern SectionManager *s_sectionManager;

void SectionManager_init(SectionManager *this);

void SectionManager_setNextSection(SectionManager *this, u32 sectionId, u32 animation);

void SectionManager_startChangeSection(SectionManager *this, s32 delay, u32 color);
