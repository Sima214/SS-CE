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
  #ifdef _WIN32
    /**
     * Marks a function which takes a format string like printf.
     */
    #define MARK_PRINTF(fmt_index, start_index) __attribute__((format(__MINGW_PRINTF_FORMAT, fmt_index, start_index)))
  #else
    /**
     * Marks a function which takes a format string like printf.
     */
    #define MARK_PRINTF(fmt_index, start_index) __attribute__((format(printf, fmt_index, start_index)))
  #endif
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
   * 'const' functions' result depend only on the value of their parameters,
   * aka they do not change the 'global' state.
   * Note that dereferencing a pointer is considered changing the 'global' state.
   */
  #define MARK_CONST __attribute__((const))
#endif
#ifndef MARK_PURE
  /**
   * Marks the functions as 'pure' which helps the compiler.
   * 'pure' functions' result depend only on the value of their parameters,
   * but are more relaxed about global state and pointers.
   */
  #define MARK_PURE __attribute__((pure))
#endif
#ifndef MARK_HOT
  /**
   * Marks a function as 'hot'.
   */
  #define MARK_HOT __attribute__((hot))
#endif
#ifndef MARK_COLD
  /**
   * Marks a function as 'cold'.
   */
  #define MARK_COLD __attribute__((cold))
#endif
#ifndef MASK_CREATE
  /**
   * Generates a mask with the bit-th bit turned on.
   */
  #define MASK_CREATE(bit) (0x1ll << (bit))
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
  #if defined(_WIN32)
    #ifndef SSCE_EXPORTS
      /**
       * Export symbol to linker.
       * Only affects shared builds.
       */
      #define EXPORT_API __declspec(dllimport)
    #else
      /**
       * Export symbol to linker.
       * Only affects shared builds.
       */
      #define EXPORT_API __declspec(dllexport)
    #endif
  #else
    /**
     * Export symbol to linker.
     * Only affects shared builds.
     */
    #define EXPORT_API __attribute__((visibility("default")))
  #endif
#endif
#ifndef EXPORT_API_RUNTIME
  #if defined(LINK_STATIC)
    /**
     * Defines a function which gets resolved at runtime.
     */
    #define EXPORT_API_RUNTIME
  #elif defined(LINK_ELF)
    /**
     * Defines a function which gets resolved at runtime.
     */
    #define EXPORT_API_RUNTIME(resolver) __attribute__((ifunc(#resolver)))
  #elif defined(LINK_MACHO)
    /**
     * Defines a function which gets resolved at runtime.
     */
    #define EXPORT_API_RUNTIME(name)                          \
      void* name ## _macho_resolver(void) __asm__("_" #name); \
      void* name ## _macho_resolver(void) {                   \
        __asm__(".symbol_resolver _" #name);                  \
        return resolve_ ## name();                            \
      }
  #elif defined(LINK_PE)
    /**
     * Defines a function which gets resolved at runtime.
     */
    #define EXPORT_API_RUNTIME
  #endif
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
    #define ssce_mkdir(dir)       \
      ({                          \
        mode_t mask = umask(0);   \
        umask(mask);              \
        mkdir(dir, 0777 & ~mask); \
      })
  #elif defined(_WIN32)
    #include <direct.h>
    /**
     * Creates a directory with default privileges.
     * 
     * @param dir The directory to create.
     * @return Returns zero on success.
     */
    #define ssce_mkdir(dir) _mkdir(dir)
  #endif
#endif
#ifndef ssce_localtime
  #if IS_POSIX
    /**
     * Converts time into a time table in a thread safe way.
     * Returns zero on success.
     */
    #define ssce_localtime(time, table) (localtime_r(&time, &table) == NULL)
  #elif defined(_WIN32)
    /**
     * Converts time into a time table in a thread safe way.
     * Returns zero on success.
     */
    #define ssce_localtime(time, table) localtime_s(&table, &time)
  #else
    #error Unsupported OS
  #endif
#endif
#ifndef cpu_init
  /**
   * Caches cpu info.
   */
  #define cpu_init __builtin_cpu_init
#endif
#ifndef EARLY_TRACE
  #ifndef NDEBUG
    #include <stdio.h>
    /**
     * Low level trace function.
     * Simply prints out msg onto the console.
     * Must contain only basic ascii symbols.
     */
    #define EARLY_TRACE(msg) puts(msg)
  #else
    /**
     * NOP on release builds.
     */
    #define EARLY_TRACE(msg)
  #endif
#endif
#ifndef EARLY_TRACEF
  #ifndef NDEBUG
    #include <stdio.h>
    /**
     * Low level trace function with formatting support.
     * Must contain only basic ascii symbols.
     */
    #define EARLY_TRACEF(...) printf(__VA_ARGS__);puts("")
  #else
    /**
     * NOP on release builds.
     */
    #define EARLY_TRACEF(...)
  #endif
#endif
#ifndef TARGET_EXT
  #if defined(NDEBUG) && defined(__clang__)
    /**
     * Mark this function as 'hand coded' for target instruction extension 'ext'.
     * Instructs the compiler to generate code for target extension while
     * also minimizing compiler optimizations.
     */
    #define TARGET_EXT(ext) __attribute__((__target__(#ext), minsize))
  #elif defined(NDEBUG) && defined(__GNUC__)
    /**
     * Mark this function as 'hand coded' for target instruction extension 'ext'.
     * Instructs the compiler to generate code for target extension while
     * also minimizing compiler optimizations.
     */
    #define TARGET_EXT(ext) __attribute__((__target__(#ext), optimize("no-tree-vectorize")))
  #elif !defined(NDEBUG)
    /**
     * Mark this function as 'hand coded' for target instruction extension 'ext'.
     * Instructs the compiler to generate code for target extension while
     * also minimizing compiler optimizations.
     */
    #define TARGET_EXT(ext) __attribute__((__target__(#ext)))
  #else
    #warning Unknown compiler: generated code might not be optimal!
    /**
     * Mark this function as 'hand coded' for target instruction extension 'ext'.
     * Instructs the compiler to generate code for target extension while
     * also minimizing compiler optimizations.
     */
    #define TARGET_EXT(ext)
  #endif
#endif
#ifndef COLD_BRANCH
  /**
   * Marks a branch as unlikely.
   * Use only when compiler is beeing stupid.
   */
  #define COLD_BRANCH(cond) __builtin_expect(cond, 0)
#endif
#ifndef HOT_BRANCH
  /**
   * Marks a branch as likely.
   * Use only when compiler is beeing stupid.
   */
  #define HOT_BRANCH(cond) __builtin_expect(cond, 1)
#endif
#ifndef MARK_MALLOC
  /**
   * Marks that a function allocates memory and returns a pointer to said memory.
   */
  #define MARK_MALLOC(alignment, ...) __attribute__((__malloc__, alloc_align(alignment), alloc_size(__VA_ARGS__)))
#endif
#ifndef MARK_MALLOC_SIMPLE
  /**
   * Marks that a function allocates memory and returns a pointer to said memory.
   */
  #define MARK_MALLOC_SIMPLE(...) __attribute__((__malloc__, alloc_size(__VA_ARGS__)))
#endif
#ifndef MARK_OBJ_ALLOC
  /**
   * Marks that a function that allocates memory for an opaque data type and returns a pointer to the new object.
   */
  #define MARK_OBJ_ALLOC __attribute__((__malloc__, warn_unused_result))
#endif
#ifndef FORCE_INLINE
  #ifndef NDEBUG
    /**
     * Forces a function to be always be inlined in optimized builds.
     */
    #define FORCE_INLINE
  #else
    /**
     * Forces a function to be always be inlined in optimized builds.
     */
    #define FORCE_INLINE __attribute__((always_inline))
  #endif
#endif
#ifndef CREATE_CLEANUP_FUNC
  /**
   * Defines a function which can be used with the cleanup attribute.
   */
  #define CREATE_CLEANUP_FUNCP(type, func) \
    static inline void func##_##type(type** p) { \
      func(*p);                           \
    }
#endif
#ifndef MARK_NONNULL_ARGS
  /**
   * Marks arguments of a function as non-null.
   * This generates compile time warnings.
   * Indexing starts at 1.
   */
  #define MARK_NONNULL_ARGS(...) __attribute__((nonnull(__VA_ARGS__)))
#endif