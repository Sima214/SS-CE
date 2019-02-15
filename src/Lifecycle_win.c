/**
 * Windows dll initialization.
 */
#include <Config.h>
#include <Modules.h>
#include <clock/Clock.h>
#include <logger/Logger.h>
#include <string/Strings.h>

#include <windows.h>

static void ssce_init() {
  #ifndef NDEBUG
    native_puts("Loading shared library ssce[" SSCE_VERSION "]");
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_init();
  #endif
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    setup_log_file();
  #endif
}

static void ssce_exit() {
  #if defined(MODULE_LOGGER) && defined(MODULE_LOGGER_FILE)
    close_log_file();
  #endif
  #if defined(MODULE_CLOCK)
    internal_clock_exit();
  #endif
  #ifndef NDEBUG
    native_puts("Unloaded shared library ssce[" SSCE_VERSION "]");
  #endif
}

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