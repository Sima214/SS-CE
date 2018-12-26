/**
 * @file
 * @brief Usefull macros.
 */
#ifndef C_DECLS_START
  #ifdef __cplusplus
    /**
     * Should be used before the inclusion of a C header file in C++ source code.
     */
    #define C_DECLS_START extern "C" {
  #else
    /**
     * Should be used before the inclusion of a C header file in C++ source code.
     */
    #define C_DECLS_START
  #endif
#endif
#ifndef C_DECLS_END
  #ifdef __cplusplus
    /**
     * Should be used after the inclusion of a C header file in C++ source code.
     */
    #define C_DECLS_END }
  #else
    /**
     * Should be used after the inclusion of a C header file in C++ source code.
     */
    #define C_DECLS_END
  #endif
#endif
#ifndef IS_POSIX
  /**
   * True if compiling for a posix compliant platform.
   */
  #define IS_POSIX (defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)))
#endif
#ifndef MARK_PRINTF
  /**
   * Marks a function which takes a format string like printf.
   */
  #define MARK_PRINTF(fmt_index, start_index) __attribute__((format(printf, fmt_index, start_index)))
#endif
#ifndef MARK_UNUSED
  /**
   * Suppresses 'not used' type warning messages.
   */
  #define MARK_UNUSED __attribute__((unused))
#endif
#ifndef MARK_PACKED
  /**
   * Forces the compiler to not add padding to structures.
   */
  #define MARK_PACKED __attribute__((packed))
#endif
#ifndef MARK_CONST
  /**
   * Marks the functions as 'const' which helps the compiler.
   * 'const' functions' result depend only on the value of their parameters.
   */
  #define MARK_CONST __attribute__((const))
#endif
#ifndef MASK_CREATE
  /**
   * Generates a mask with the bit-th bit turned on.
   */
  #define MASK_CREATE(bit) (0x1 << (bit))
#endif
#ifndef MASK_TEST
  /**
   * Tests if all the bits of the mask are turned on in the input.
   */
  #define MASK_TEST(o, m) (!!(o & m))
#endif
#ifndef MASK_SET
  /**
   * Sets or unsets bits based on the mask according to the boolean
   * value v. The value of the first parameter is overwritten.
   */
  #define MASK_SET(o, m, v) o = ((~m) & o) | (v ? m : 0x0)
#endif
#ifndef MASK_TOOGLE
  /**
   * Toogles all the bits of the first parameter based on the mask.
   */
  #define MASK_TOOGLE(o, m) o ^= m
#endif
#ifndef EXPORT_API
  /**
   * Export symbol to linker.
   * Only affects shared builds.
   */
  #define EXPORT_API __attribute__((visibility("default")))
#endif
#ifndef EXPORT_API_RUNTIME
  /**
   * Defines a function which gets resolved at runtime.
   */
  #define EXPORT_API_RUNTIME(resolver) __attribute__((ifunc(#resolver)))
#endif
#ifndef strequal
  /**
   * Tests if two strings are equal.
   */
  #define strequal(s1, s2) (strcmp(s1, s2) == 0)
#endif
#ifndef ssce_mkdir
  #if IS_POSIX
    #include <sys/stat.h>
    /**
     * Creates a directory with default privileges.
     * 
     * @param dir The directory to create.
     * @return Returns zero on success.
     */
    #define ssce_mkdir(dir) mkdir(dir, 0664)
  #elif defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    /**
     * Creates a directory with default privileges.
     * 
     * @param dir The directory to create.
     * @return Returns zero on success.
     */
    #define ssce_mkdir(dir) _mkdir(dir)
  #endif
#endif
#ifndef cpu_init
  /**
   * Caches cpu info.
   */
  #define cpu_init __builtin_cpu_init
#endif