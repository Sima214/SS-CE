/**
 * Windows dll initialization.
 */
#include "ntdll.h"

#include <Config.h>
#include <Macros.h>
#include <Modules.h>
#include <clock/Clock.h>
#include <logger/Logger.h>
#include <memory/FAlloc.h>

#include <windows.h>

/*
 * Implement ntdll IAT.
 * Done manually, because ntdll does not export its functions to the linker.
 */
static NT_DELAY_EXECUTION pNtDelayExecution = NULL;
static NT_QUERY_TIMER_RESOLUTION pNtQueryTimerResolution = NULL;
static NT_SET_TIMER_RESOLUTION pNtSetTimerResolution = NULL;

NTSTATUS WINAPI NtDelayExecution(BOOLEAN alertable, const LARGE_INTEGER* delayInterval) {
  return (*pNtDelayExecution)(alertable, delayInterval);
}

NTSTATUS WINAPI NtQueryTimerResolution(PULONG minimumResolution, PULONG maximumResolution, PULONG currentResolution) {
  return (*pNtQueryTimerResolution)(minimumResolution, maximumResolution, currentResolution);
}

NTSTATUS WINAPI NtSetTimerResolution(ULONG desiredResolution, BOOLEAN setResolution, PULONG currentResolution) {
  return (*pNtSetTimerResolution)(desiredResolution, setResolution, currentResolution);
}

/*
 * Get ntdll's function addresses.
 */
static void link_ntdll() {
  HMODULE ntdll = GetModuleHandleA("ntdll");
  // Null assert.
  if(COLD_BRANCH(ntdll == NULL)) {
    EARLY_TRACE("Could not find ntdll module!");
    abort();
  }
  // Find NtDelayExecution.
  pNtDelayExecution = (NT_DELAY_EXECUTION) GetProcAddress(ntdll, "NtDelayExecution");
  if(COLD_BRANCH(pNtDelayExecution == NULL)) {
    EARLY_TRACE("Could not find NtDelayExecution in ntdll!");
    abort();
  }
  // Find NtQueryTimerResolution.
  pNtQueryTimerResolution = (NT_QUERY_TIMER_RESOLUTION) GetProcAddress(ntdll, "NtQueryTimerResolution");
  if(COLD_BRANCH(pNtQueryTimerResolution == NULL)) {
    EARLY_TRACE("Could not find NtQueryTimerResolution in ntdll!");
    abort();
  }
  // Find NtSetTimerResolution.
  pNtSetTimerResolution = (NT_SET_TIMER_RESOLUTION) GetProcAddress(ntdll, "NtSetTimerResolution");
  if(COLD_BRANCH(pNtSetTimerResolution == NULL)) {
    EARLY_TRACE("Could not find NtSetTimerResolution in ntdll!");
    abort();
  }
}

/*
 * Early init procedure for Win32.
 */
static void ssce_init() {
  EARLY_TRACE("Loading shared library ssce[" SSCE_VERSION "]");
  link_ntdll();
  #if defined(MODULE_MEMORY)
    internal_falloc_init();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER)
    internal_logger_init();
  #endif
}

/*
 * Exit procedure for Win32.
 */
static void ssce_exit() {
  #if defined(MODULE_LOGGER)
    internal_logger_exit();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
  #endif
  #if defined(MODULE_MEMORY)
    internal_falloc_exit();
  #endif
  EARLY_TRACE("Unloaded shared library ssce[" SSCE_VERSION "]");
}

/*
 * Part of 'Dll' ABI.
 */
BOOL WINAPI DllMain(MARK_UNUSED HINSTANCE hinstDLL, MARK_UNUSED DWORD fdwReason, MARK_UNUSED LPVOID lpvReserved) {
  switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
      ssce_init();
      break;
    case DLL_PROCESS_DETACH:
      ssce_exit();
      break;
  }
  return TRUE;
}