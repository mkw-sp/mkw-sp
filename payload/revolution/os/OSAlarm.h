#pragma once

#include "revolution/os/OSContext.h"

typedef struct OSAlarm OSAlarm;

typedef void (*OSAlarmHandler)(OSAlarm *alarm, OSContext *context);

struct OSAlarm {
    u8 _00[0x2c - 0x00];
};
static_assert(sizeof(OSAlarm) == 0x2c);

void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler);
