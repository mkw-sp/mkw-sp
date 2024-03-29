#pragma once

#include <Common.h>

typedef u16 OSError;

#define OS_ERROR_SYSTEM_RESET_EXCEPTION 0
#define OS_ERROR_MACHINE_CHECK_EXCEPTION 1
#define OS_ERROR_DATA_STORAGE_INTERRUPT_EXCEPTION 2
#define OS_ERROR_INSTRUCTION_STORAGE_INTERRUPT_EXCEPTION 3
#define OS_ERROR_EXTERNAL_INTERRUPT_EXCEPTION 4
#define OS_ERROR_ALIGNMENT_EXCEPTION 5
#define OS_ERROR_PROGRAM_EXCEPTION 6
#define OS_ERROR_FLOATING_POINT_UNAVAILABLE_EXCEPTION 7
#define OS_ERROR_DECREMENTER_EXCEPTION 8
#define OS_ERROR_SYSTEM_CALL_EXCEPTION 9
#define OS_ERROR_TRACE_EXCEPTION 10
#define OS_ERROR_PERFORMACE_MONITOR_EXCEPTION 11
#define OS_ERROR_INSTRUCTION_ADDRESS_BREAKPOINT_EXCEPTION 12
#define OS_ERROR_SYSTEM_MANAGEMENT_INTERRUPT_EXCEPTION 13
#define OS_ERROR_THERMAL_INTERRUPT_EXCEPTION 14
#define OS_ERROR_MEMORY_PROTECTION_VIOLATION 15
#define OS_ERROR_FLOATING_POINT_EXCEPTION 16
#define OS_ERROR_MAX 17
