#pragma once

#include <Common.h>

//! 1. Search for a GCKeyboard device in slots 0-3
//! 2. If found, try to connect to it
//! 3. If connected, enable the background service to grab keypresses
void SIKeyboard_InitSimple(void);

//! Search for a channel index with a GCKeyboard inserted. Return -1 if none.
s32 SIKeyboard_Scan(void);

void SIKeyboard_Connect(u32 channel);
void SIKeyboard_Disconnect(void);

s32 SIKeyboard_GetCurrentConnection(void);

// Polling
size_t SIKeyboard_Poll(char *keys, size_t max_keys);

// Interrupts
//
// Call once, and keyboard events will be buffered in the background.
bool SIKeyboard_EnableBackgroundService(void);
size_t SIKeyboard_ConsumeBuffer(char *buf, size_t max_take);
