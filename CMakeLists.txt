cmake_minimum_required( VERSION 3.6 )
if( NOT CMAKE_BUILD_TYPE )
	message( STATUS "Setting build type to 'Release' as none was specified." )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
	## Set the possible values of build type for cmake-gui
	SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" )
endif()

function(declare_module modname doc)
    ## Create docstring for dependencies.
    set( deps "" )
    if(NOT "${ARGN}" STREQUAL "")
        set( deps "\nDepends on: " )
    endif()
    foreach(d ${ARGN})
        set( deps "${deps}${d}, " )
    endforeach(d)
    ## Remove trailing comma and space.
    if(NOT "${ARGN}" STREQUAL "")
        string( LENGTH ${deps} deps_len )
        string( SUBSTRING ${deps} 0 ${deps_len}-2 ${deps} )
    endif()
    ## Load variable from the cache.
    set( ${modname} TRUE CACHE BOOL "${doc}${deps}" )
    ## Check dependencies.
    foreach(d ${ARGN})
        if(NOT ${${d}})
            message( SEND_ERROR "${modname} requires ${d}, but ${d} is disabled!" )
        endif(NOT ${${d}})
    endforeach(d)
endfunction()

function(define_module modname src inc)
    if(${${modname}})
        message( STATUS "${modname}:\tON" )
        set( SSCE_SRC "${SSCE_SRC}\;${src}" PARENT_SCOPE )
        set( SSCE_INC "${SSCE_INC}\;${inc}" PARENT_SCOPE )
        file( APPEND "${PROJECT_SOURCE_DIR}/src/Modules.h" "#define ${modname}\n" )
    else(${${modname}})
        message( STATUS "${modname}:\tOFF" )
    endif(${${modname}})
endfunction()

function(define_test modname test)
    string( REGEX REPLACE "MODULE_" "" name "${modname}" )
    string( TOLOWER "${name}" name )
    message( STATUS "Adding tests for ${name}." )
    add_executable( "${PROJECT_NAME}_${name}_test" ${test} )
    include_directories( "${SSCE_DIR_SRC}" )
    target_link_libraries( "${PROJECT_NAME}_${name}_test" ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )
    add_test( NAME ${name} COMMAND "${PROJECT_NAME}_${name}_test" )
endfunction()

## Configure project and settings.
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 1.1 )
set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}" )
set( SSCE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( SSCE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( SSCE_VERSION ${PROJECT_VERSION} )
set( PROJECT_NAME "ssce")
project( ${PROJECT_NAME} VERSION ${SSCE_VERSION} DESCRIPTION "A random C/C++ library." )
set( SSCE_DIR_SRC "${PROJECT_SOURCE_DIR}/src" )
set( SSCE_DIR_TEST "${PROJECT_SOURCE_DIR}/test" )

## Module selection.
declare_module( "MODULE_MATH" "Random math utilities." )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module." )
declare_module( "MODULE_THREADS" "Enable posix thread support." )
declare_module( "MODULE_STRINGS" "Enable strings util functions." )
declare_module( "MODULE_LOGGER" "Enable the logger module." "MODULE_STRINGS" "MODULE_THREADS" )
declare_module( "MODULE_ARGPARSE" "Enable the argument parser module." )
declare_module( "MODULE_STRUCT" "Enable the data structures module." )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_SOURCE_DIR}/src/Config.h" )
file( WRITE "${PROJECT_SOURCE_DIR}/src/Modules.h" "/*\n * Auto generated from CMakeLists.txt.\n * This holds the definitions of the enabled modules.\n */\n" )

## Platform detection.
if( WIN32 )
    message( "Compiling for windows." )
    set( SSCE_PLT "win" )
elseif( UNIX AND NOT APPLE )
    message( "Compiling for linux." )
    set( SSCE_PLT "linux" )
    add_definitions( "-D_GNU_SOURCE" )
elseif( APPLE )
    message( "Compiling for macos." )
    set( SSCE_PLT "mac" )
else()
    message( FATAL_ERROR "Unsupported os!!!" )
endif()

## Module definitions.
set( SSCE_SRC "${SSCE_DIR_SRC}/Lifecycle_${SSCE_PLT}.c" )
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h\;${SSCE_DIR_SRC}/Modules.h" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
define_module( "MODULE_MATH" "" "${SSCE_DIR_SRC}/math/MinMax.h\;${SSCE_DIR_SRC}/math/MinMax.hpp" )
define_module( "MODULE_CLOCK" "${SSCE_DIR_SRC}/clock/Clock_${SSCE_PLT}.c" "${SSCE_DIR_SRC}/clock/Clock.h\;${SSCE_DIR_SRC}/clock/Clock.hpp" )
define_module( "MODULE_THREADS" "" "" )
define_module( "MODULE_STRINGS" "${SSCE_DIR_SRC}/string/Strings_${SSCE_PLT}.c;${SSCE_DIR_SRC}/string/Strings.c" "${SSCE_DIR_SRC}/string/Strings.h\;${SSCE_DIR_SRC}/string/Strings.hpp" )
define_module( "MODULE_LOGGER" "${SSCE_DIR_SRC}/logger/Logger.c" "${SSCE_DIR_SRC}/logger/Logger.h\;${SSCE_DIR_SRC}/logger/Logger.hpp" )
define_module( "MODULE_ARGPARSE" "${SSCE_DIR_SRC}/argparse/ArgParse.c" "${SSCE_DIR_SRC}/argparse/ArgParse.h\;${SSCE_DIR_SRC}/argparse/ArgParse.hpp" )
define_module( "MODULE_STRUCT" "${SSCE_DIR_SRC}/structures/Heap.c\;${SSCE_DIR_SRC}/structures/Sort.c" "${SSCE_DIR_SRC}/structures/Interface.h\;${SSCE_DIR_SRC}/structures/Sort.h\;${SSCE_DIR_SRC}/structures/Sort.hpp\;${SSCE_DIR_SRC}/structures/Heap.h\;${SSCE_DIR_SRC}/structures/Heap.hpp" )
## End of modules definitions

## Define the library.
## TODO: jemalloc, cflags for ssce.pc, travis ci integration(x3)
set( SHARED TRUE CACHE BOOL "Disable to build static libraries." )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -masm=intel -Wall -Wextra -Wno-expansion-to-defined" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )
if(${SHARED})
    message( STATUS "Building shared library." )
    add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
else()
    message( STATUS "Building static library." )
    add_library( ${PROJECT_NAME} STATIC ${SSCE_SRC} )
endif()
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${SSCE_INC} )
target_include_directories( ${PROJECT_NAME} PRIVATE "${SSCE_DIR_SRC}" )

## Uninstall.
if(NOT TARGET uninstall)
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
    add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
endif()

## Test configuration.
enable_testing()
find_package( Threads )
define_test( "MODULE_STRUCT" "${SSCE_DIR_TEST}/struct.c" )

## Define installation.
include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
