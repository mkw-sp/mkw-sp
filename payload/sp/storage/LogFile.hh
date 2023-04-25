#pragma once

#include <stdarg.h>

namespace SP::LogFile {

void Init();
void VPrintf(const char *format, va_list args);

} // namespace SP::LogFile
