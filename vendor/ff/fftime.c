#include "ff.h"

#include <revolution.h>

DWORD get_fattime (void)
{
	OSCalendarTime time;
	OSTicksToCalendarTime(OSGetTime(), &time);

	DWORD val = 0;

	assert(time.year >= 1980 && time.year < 2108);
	val |= (time.year - 1980);

	assert(time.mon >= 0 && time.mon < 12);
	val |= (time.mon + 1) << 21;

	assert(time.mday >= 1 && time.mday <= 31);
	val |= time.mday << 16;

	assert(time.hour >= 0 && time.hour < 24);
	val |= time.hour << 11;

	assert(time.min >= 0 && time.min < 60);
	val |= time.min << 5;

	assert(time.sec >= 0 && time.sec < 62);
	if (time.sec >= 60) {
		val |= 59 >> 1;
	} else {
		val |= time.sec >> 1;
	}

	return val;
}
