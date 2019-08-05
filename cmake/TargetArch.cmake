# Based on the Qt 5 processor detection code, so should be very accurate
# https://qt.gitorious.org/qt/qtbase/blobs/master/src/corelib/global/qprocessordetection.h
# Later taken from https://github.com/axr/solar-cmake
# Cleaned up and modified the code for SS-CE's build system.
# This version only handles arm and x86 (32/64).

set(ARCHDETECT_C_CODE "
#if defined(__arm__) || defined(__TARGET_ARCH_ARM)
    #if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || \\
        (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
        #error cmake_ARCH arm
    #elif defined(__aarch64__) || defined(__arm64__) || defined(__ARM_ARCH_8A__)
        #error cmake_ARCH arm64
    #else
        #error CMAKE_ARCH unknown
    #endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error CMAKE_ARCH i386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error CMAKE_ARCH x86_64
#else
    #error CMAKE_ARCH unknown
#endif
")

function(target_architecture output_var)
    file( WRITE "${CMAKE_BINARY_DIR}/generated/arch.c" "${ARCHDETECT_C_CODE}" )

    # Detect the architecture in a rather creative way...
    # This compiles a small C program which is a series of ifdefs that selects a
    # particular #error preprocessor directive whose message string contains the
    # target architecture. The program will always fail to compile (both because
    # file is not a valid C program, and obviously because of the presence of the
    # #error preprocessor directives... but by exploiting the preprocessor in this
    # way, we can detect the correct target architecture even when cross-compiling,
    # since the program itself never needs to be run (only the compiler/preprocessor)
    try_run(
        run_result_unused
        compile_result_unused
        "${CMAKE_BINARY_DIR}"
        "${CMAKE_BINARY_DIR}/generated/arch.c"
        COMPILE_OUTPUT_VARIABLE arch
    )

    # Parse the architecture name from the compiler output
    string( REGEX MATCH "CMAKE_ARCH ([a-zA-Z0-9_]+)" arch "${arch}" )

    # Get rid of the value marker leaving just the architecture name
    string( REPLACE "CMAKE_ARCH " "" arch "${arch}" )

    # If we are compiling with an unknown architecture this variable should
    # already be set to "unknown" but in the case that it's empty (i.e. due
    # to a typo in the code), then set it to unknown
    if( NOT arch )
        set( arch unknown )
    endif()
    set( ${output_var} "${arch}" PARENT_SCOPE )

    add_compile_definitions( "${arch}" )

    message( STATUS "Compiling for ${arch}." )
endfunction()
