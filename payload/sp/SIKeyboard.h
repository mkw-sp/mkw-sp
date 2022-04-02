#pragma once

#include <Common.h>

void SIKeyboard_InitSimple(void);

s32 SIKeyboard_Scan(void);

void SIKeyboard_Init(u32 channel);
void SIKeyboard_Terminate(void);
s32 SIKeyboard_GetActiveChannel(void);

// Polling
size_t SIKeyboard_Poll(char *keys, size_t max_keys);

// Interrupts
bool SIKeyboard_EnableBackgroundService(void);
size_t SIKeyboard_ConsumeBuffer(char *buf, size_t max_take);
