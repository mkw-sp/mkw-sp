#pragma once

#include <Common.h>

// Channels
#define OS_PROTECT_CHANNEL_0 0
#define OS_PROTECT_CHANNEL_1 1
#define OS_PROTECT_CHANNEL_2 2
#define OS_PROTECT_CHANNEL_3 3

// Permissions
#define OS_PROTECT_PERMISSION_NONE  0x00
#define OS_PROTECT_PERMISSION_READ  0x01
#define OS_PROTECT_PERMISSION_WRITE 0x02
#define OS_PROTECT_PERMISSION_RW    (OS_PROTECT_PERMISSION_READ | OS_PROTECT_PERMISSION_WRITE)

void OSProtectRange(u32 channel, void *address, u32 size, u32 permissions);

void RealMode(void (*function)(void));
void BATConfig(void);
void __OSInitMemoryProtection(void);
