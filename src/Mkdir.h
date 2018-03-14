/*
 * Creates a folder.
 * Cross-platform compatible.
 */
#include <Macros.h>
#if IS_POSIX
  #include <sys/stat.h>
  #define ssce_mkdir(dir) mkdir(dir, 0664)
#elif defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define ssce_mkdir(dir) _mkdir(dir)
#endif