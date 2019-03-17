# Initialize cmake.
cmake_minimum_required( VERSION 3.9.4 )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
if( NOT CMAKE_BUILD_TYPE )
	message( STATUS "Setting build type to 'Release' as none was specified." )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
	## Set the possible values of build type for cmake-gui
	SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" )
endif()

find_package( PythonInterp 3 REQUIRED )
include( cmake/TargetPlatform.cmake )
include( cmake/SSCEModules.cmake )
include( cmake/TargetArch.cmake )
include( CheckCCompilerFlag )
include( ExternalProject )
find_package( PkgConfig )

## Configure project and settings.
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 1 )
set( PROJECT_VERSION_RELEASE 1 )
set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_RELEASE}" )
set( SSCE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( SSCE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( SSCE_VERSION_RELEASE ${PROJECT_VERSION_RELEASE} )
set( SSCE_VERSION ${PROJECT_VERSION} )
set( PROJECT_NAME "ssce")
project( ${PROJECT_NAME} VERSION ${SSCE_VERSION} DESCRIPTION "A random C/C++ library." )
set( SSCE_DIR_SRC "${PROJECT_SOURCE_DIR}/src" )
set( SSCE_DIR_TEST "${PROJECT_SOURCE_DIR}/test" )
set( SSCE_DIR_DOCS "${PROJECT_SOURCE_DIR}/docs" )

## Platform detection.
target_platform( SSCE_PLT DYNLIB_EXT )

# Architecture detection.
target_architecture( SSCE_ARCH )

## Module selection.
declare_module( "MODULE_MATH" "Enable extra math functions" )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module" )
declare_module( "MODULE_MEMORY" "Enable memory manipulation utilities" )
declare_module( "MODULE_STRING" "Enable strings util functions" "MODULE_MEMORY" )
declare_module( "MODULE_STRUCTURES" "Enable the data structures module" "MODULE_STRING" )
declare_module( "MODULE_LOGGER" "Enable the logger module" "MODULE_MEMORY" "MODULE_STRINGS" "MODULE_STRUCTURES" )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
set( MODULE_LOGGER_FILE_PREFIX "logs/${PROJECT_NAME}_" CACHE STRING "If logger files are enabled, this sets the path prefix of the file." )
set( MODULE_MEMORY_FALLOC_STACK_SIZE "8*1024" CACHE STRING "How much kibibytes to allocate initially for each thread local stack. Note that reallocations are not possible, so choose a large size here." )
set( MODULE_TESTING TRUE CACHE BOOL "Whether to enable tests. Disabling may fix some build issues." )
set( SYSTEM_JEMALLOC FALSE CACHE BOOL "Link against system jemalloc library." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_SOURCE_DIR}/src/Config.h" )
configure_file( "${PROJECT_SOURCE_DIR}/src/Modules.h.in" "${PROJECT_SOURCE_DIR}/src/Modules.h" )

## Module definitions.
include_directories( "${SSCE_DIR_SRC}/core" )
set( SSCE_SRC "${SSCE_DIR_SRC}/core/Lifecycle_${SSCE_PLT}.c;${SSCE_DIR_SRC}/core/Runtime_${SSCE_ARCH}.c;${SSCE_DIR_SRC}/core/Runtime.c" )
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h;${SSCE_DIR_SRC}/Modules.h;${SSCE_DIR_SRC}/Config.h;${SSCE_DIR_SRC}/core/Runtime.h;${SSCE_DIR_SRC}/core/Runtime.hpp" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
define_module( "MODULE_MATH" "" "MinMax.h;MinMax.hpp" )
define_module( "MODULE_CLOCK" "Clock_${SSCE_PLT}.c" "Clock.h;Clock.hpp" )
define_module( "MODULE_MEMORY" "Swap_${SSCE_ARCH}.c;GAlloc_${SSCE_PLT}.c;FAlloc.c" "Memory.h;Memory.hpp;FAlloc.h;GAlloc.h;GAlloc.hpp" )
define_module( "MODULE_STRING" "SStrings_${SSCE_PLT}.c;SStrings.c" "SStrings.h;SStrings.hpp" )
define_module( "MODULE_STRUCTURES" "Heap.c;Sort.c" "Interface.h;Sort.h;Sort.hpp;Heap.h;Heap.hpp" )
define_module( "MODULE_LOGGER" "Logger.c" "Logger.h;Logger.hpp" )

# Prepare external libraries.
set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )
find_package( Threads REQUIRED )
if( ${SYSTEM_JEMALLOC} )
    if( WIN32 )
        # Currently I do not build jemalloc on Windows, because of script weirdness.
        add_library( jemalloc SHARED IMPORTED )
        message( STATUS "${CMAKE_BINARY_DIR}/jemalloc/lib/jemalloc.${DYNLIB_EXT}" )
        set_target_properties( jemalloc PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/jemalloc/lib/jemalloc.${DYNLIB_EXT}" )
        include_directories( "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc" )
        set( JEMALLOC_LIBRARIES "${CMAKE_BINARY_DIR}/jemalloc/lib/jemalloc.${DYNLIB_EXT}" )
    else( WIN32 )
        message( STATUS "Using system jemalloc." )
        pkg_check_modules( JEMALLOC REQUIRED jemalloc )
        include_directories( "${JEMALLOC_INCLUDE_DIRS}/jemalloc" )
    endif( WIN32 )
else( ${SYSTEM_JEMALLOC} )
    message( STATUS "Using built-in jemalloc." )
    ExternalProject_Add( jemalloc-build
                         PREFIX "${PROJECT_SOURCE_DIR}/jemalloc"
                         SOURCE_DIR "${PROJECT_SOURCE_DIR}/jemalloc"
                         BINARY_DIR "${PROJECT_SOURCE_DIR}/jemalloc"
                         STAMP_DIR "${CMAKE_BINARY_DIR}/jemalloc/.tmp"
                         TMP_DIR "${CMAKE_BINARY_DIR}/jemalloc/.tmp"
                         URL "https://d3f3nimbmr2mxp.cloudfront.net/ssce/prebuilt/src/jemalloc.tar.bz2"
                         CONFIGURE_COMMAND sh ./configure --with-jemalloc-prefix=je_ --prefix=${CMAKE_BINARY_DIR}/jemalloc
                         BUILD_COMMAND ${CMAKE_MAKE_PROGRAM} build_lib_shared
                         INSTALL_COMMAND ${CMAKE_MAKE_PROGRAM} install_include install_lib_shared
                         TEST_EXCLUDE_FROM_MAIN 1
                         BUILD_ALWAYS 1
                        )
    add_library( jemalloc SHARED IMPORTED )
    set_target_properties( jemalloc PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/jemalloc/lib/libjemalloc.${DYNLIB_EXT}" )
    include_directories( "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc" )
    set( JEMALLOC_LIBRARIES jemalloc )
endif( ${SYSTEM_JEMALLOC} )

## Define the library.
set( BUILD_SHARED TRUE CACHE BOOL "Disable to build static libraries." )
if( ${BUILD_SHARED} )
    message( STATUS "Building shared library." )
    add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
    target_link_libraries( ${PROJECT_NAME} Threads::Threads ${JEMALLOC_LIBRARIES} )
else()
    message( STATUS "Building static library." )
    add_definitions( "-DLINK_STATIC" )
    add_library( ${PROJECT_NAME} STATIC ${SSCE_SRC} )
    target_link_libraries( ${PROJECT_NAME} Threads::Threads ${JEMALLOC_LIBRARIES} )
endif()
if( NOT ${SYSTEM_JEMALLOC} )
    add_dependencies( ${PROJECT_NAME} jemalloc-build )
endif()
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SSCE_INC}" )
if( NOT TARGET verblink )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerbLink.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/VerbLink.cmake" IMMEDIATE @ONLY )
    add_custom_target( verblink COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/VerbLink.cmake )
    add_dependencies( ${PROJECT_NAME} verblink )
endif()

## Configure compiler.
include_directories( "${SSCE_DIR_SRC}" )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_CXX_STANDARD 11 )
# Make <libname>_EXPORTS uppercase.
set_target_properties( ${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL "SSCE_EXPORTS" )
## Optimization flags.
if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    message( "Debug build detected, extra optimization flags are not enabled." )
else()
    check_c_compiler_flag( "-fno-plt" noplt )
    if( ${noplt} )
        target_compile_options( ${PROJECT_NAME} PRIVATE "-fno-plt" )
        message( STATUS "-fno-plt is supported and enabled for the library." )
    else()
        message( STATUS "-fno-plt is not supported." )
    endif()
    include( CheckIPOSupported )
    check_ipo_supported( RESULT ipo_supported OUTPUT ipo_error )
    if( ${ipo_supported} )
        message( STATUS "IPO/LTO is supported and enabled." )
        set_property( TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE )
    else()
        message(STATUS "IPO/LTO is not supported: <${ipo_error}>.")
    endif()
endif()
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-expansion-to-defined -fvisibility=hidden" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=UTF-8" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -masm=intel" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )

## Define installation.
message( STATUS "Found strip executable: ${CMAKE_STRIP}" )
include( GNUInstallDirs )
install( TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install( FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig )
if( NOT ${SYSTEM_JEMALLOC} OR WIN32 )
    # Copy jemalloc files.
    set( SSCE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}" )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyJemalloc.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CopyJemalloc.cmake" IMMEDIATE @ONLY )
    install( SCRIPT "${CMAKE_BINARY_DIR}/CopyJemalloc.cmake" )
    install( FILES "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc/jemalloc.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
endif()

## Uninstall script.
if( NOT TARGET uninstall )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
    add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
endif()

## Packaging
if( NOT TARGET package )
    add_custom_target(
        package
        ${PYTHON_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/package.py" ${CMAKE_INSTALL_PREFIX} "package"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        USES_TERMINAL BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/package.tar.xz"
        COMMENT "This may not work as intended if make install was not run before."
    )
endif()

## Test configuration.
if( MODULE_TESTING )
    enable_testing()
    define_test( "MODULE_CLOCK" "timings" )
    define_test( "MODULE_MEMORY" "swap" "galloc" "falloc" )
    define_test( "MODULE_STRING" "concat" "puts" )
    define_test( "MODULE_STRUCTURES" "heapsort" "heap" )
    define_test( "MODULE_LOGGER" "core" )
else()
    message( WARNING "Testing is not enabled!" )
endif()

## Build documentation.
find_package( Doxygen )
if( DOXYGEN_FOUND )
    ## Configure config file.
    string( REGEX REPLACE  "\;" " " DOXY_INPUT "${SSCE_INC}" )
    configure_file("${SSCE_DIR_DOCS}/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)
    ## Add make target.
    add_custom_target( doc COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation with Doxygen" VERBATIM )
else()
    message( WARNING "Doxygen need to be installed to generate the doxygen documentation" )
endif()

## Codestyle.
find_program( BUILD_LUA NAMES lua DOC "The path to the lua executable. Used for fixing preprocessor formatting." )
find_program( BUILD_FORMAT NAMES clang-format DOC "The path to clang-format." )
if( "NOT EXISTS ${BUILD_LUA}" AND "NOT EXISTS ${BUILD_FORMAT}" )
    ## Neither executables could be found.
    message( WARNING "Both lua and clang-format are required for code formatting support." )
elseif( NOT EXISTS ${BUILD_LUA} )
    ## Only lua could not be found.
    message( WARNING "lua is required for code formatting support." )
elseif( NOT EXISTS ${BUILD_FORMAT} )
    ## Only clang-format could not be found.
    message( WARNING "clang-format is required for code formatting support." )
else()
    ## Enable formatting.
    add_custom_target(style "lua" "${PROJECT_SOURCE_DIR}/style-macros.lua" ${SSCE_INC} ${SSCE_SRC} DEPENDS ${SSCE_INC} ${SSCE_SRC} )
endif()