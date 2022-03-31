#pragma once

#include <Common.h>

void CourseManager_init(void);

u32 CourseManager_count(void);

u32 CourseManager_getDbId(u32 index);

u32 CourseManager_getCourseId(u32 index);

const wchar_t *CourseManager_getPrefix(u32 index);

const wchar_t *CourseManager_getName(u32 index);
