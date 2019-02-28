#ifndef SSCE_NTDLL_H
#define SSCE_NTDLL_H
/**
 * Internal interface to the ntdll.
 * Windows only!
 * Definitions are in Lifecycle_win.c
 */

#include <windows.h>
#include <winternl.h>

/**
 * Function declarations.
 */
NTSTATUS WINAPI NtDelayExecution(BOOLEAN, const LARGE_INTEGER*);
NTSTATUS WINAPI NtQueryTimerResolution(PULONG, PULONG, PULONG);
NTSTATUS WINAPI NtSetTimerResolution(ULONG, BOOLEAN, PULONG);

/**
 * Function pointer prototypes.
 */
typedef NTSTATUS (WINAPI *NT_DELAY_EXECUTION)(BOOLEAN, const LARGE_INTEGER*);
typedef NTSTATUS (WINAPI *NT_QUERY_TIMER_RESOLUTION)(PULONG, PULONG, PULONG);
typedef NTSTATUS (WINAPI *NT_SET_TIMER_RESOLUTION)(ULONG, BOOLEAN, PULONG);

#endif /*SSCE_NTDLL_H*/