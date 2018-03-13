/*
 * A random list of macros.
 */
/*
 * The next two functions should be used before and after
 * the inclusion of a C header file in C++ source code.
 */
#ifndef C_DECLS_START
  #ifdef __cplusplus
    #define C_DECLS_START extern "C" {
  #else
    #define C_DECLS_START
  #endif
#endif
#ifndef C_DECLS_END
  #ifdef __cplusplus
    #define C_DECLS_END }
  #else
    #define C_DECLS_END
  #endif
#endif
#ifndef SSCE_MASK_CREATE
  #define SSCE_MASK_CREATE(bit) (0x1<<(bit))
#endif