/**
 * Windows dll initialization.
 */
#include <Config.h>
#include <Macros.h>
#include <Modules.h>
#include <clock/Clock.h>
#include <logger/Logger.h>

#include <windows.h>

/*
 * Early init procedure for Win32.
 */
static void ssce_init() {
  EARLY_TRACE("Loading shared library ssce[" SSCE_VERSION "]");
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    setup_log_file();
  #endif
}

/*
 * Exit procedure for Win32.
 */
static void ssce_exit() {
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    close_log_file();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
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