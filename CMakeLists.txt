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
    ## Append module definition.
    if(${${modname}})
        set( MODULE_DEFINITIONS "${MODULE_DEFINITIONS}#define ${modname}\n" PARENT_SCOPE )
    endif(${${modname}})
    ## Check dependencies.
    if(${${modname}})
        foreach(d ${ARGN})
            if(NOT ${${d}})
                message( SEND_ERROR "${modname} requires ${d}, but ${d} is disabled!" )
            endif(NOT ${${d}})
        endforeach(d)
    endif(${${modname}})
endfunction()

function(define_module modname src inc)
    if(${${modname}})
        message( STATUS "${modname}:\tON" )
        string( REGEX REPLACE "MODULE_" "" namestripped ${modname} )
        string( TOLOWER ${namestripped} folder )
        set( folder "${SSCE_DIR_SRC}/${folder}" )
        include_directories( ${folder} )
        foreach(cc ${src})
            list( APPEND srclist "${folder}/${cc}" )
        endforeach(cc ${src})
        set( SSCE_SRC ${SSCE_SRC} ${srclist} PARENT_SCOPE )
        foreach(ch ${inc})
            list( APPEND headerlist "${folder}/${ch}" )
        endforeach(ch ${inc})
        set( SSCE_INC ${SSCE_INC} ${headerlist} PARENT_SCOPE )
    else(${${modname}})
        message( STATUS "${modname}:\tOFF" )
    endif(${${modname}})
endfunction()

function(define_test modname)
    if(${${modname}})
        string( REGEX REPLACE "MODULE_" "" name "${modname}" )
        string( TOLOWER "${name}" name )
        message( STATUS "Adding tests for ${name}:" )
        foreach(d ${ARGN})
            message( STATUS "\tAdding ${d} test." )
            set( subname "${name}_${d}" )
            add_executable( "test_${subname}" "${SSCE_DIR_TEST}/${subname}.c" )
            target_link_libraries( "test_${subname}" ${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )
            add_test( NAME ${subname} COMMAND "test_${subname}" )
        endforeach(d)
    endif(${${modname}})
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
set( SSCE_DIR_DOCS "${PROJECT_SOURCE_DIR}/docs" )

## Module selection.
declare_module( "MODULE_MATH" "Random math utilities." )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module." )
declare_module( "MODULE_THREADS" "Enable posix thread support." )
declare_module( "MODULE_STRING" "Enable strings util functions." )
declare_module( "MODULE_LOGGER" "Enable the logger module." "MODULE_STRINGS" "MODULE_THREADS" )
declare_module( "MODULE_ARGPARSE" "Enable the argument parser module." )
declare_module( "MODULE_STRUCTURES" "Enable the data structures module." )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_SOURCE_DIR}/src/Config.h" )
configure_file( "${PROJECT_SOURCE_DIR}/src/Modules.h.in" "${PROJECT_SOURCE_DIR}/src/Modules.h" )

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
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h;${SSCE_DIR_SRC}/Modules.h" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
define_module( "MODULE_MATH" "" "MinMax.h;MinMax.hpp" )
define_module( "MODULE_CLOCK" "Clock_${SSCE_PLT}.c" "Clock.h;Clock.hpp" )
define_module( "MODULE_THREADS" "" "" )
define_module( "MODULE_STRING" "Strings_${SSCE_PLT}.c;Strings.c" "Strings.h;Strings.hpp" )
define_module( "MODULE_LOGGER" "Logger.c" "Logger.h;Logger.hpp" )
define_module( "MODULE_ARGPARSE" "ArgParse.c" "ArgParse.h;ArgParse.hpp" )
define_module( "MODULE_STRUCTURES" "Heap.c;Sort.c" "Interface.h;Sort.h;Sort.hpp;Heap.h;Heap.hpp" )

## End of modules definitions.

## Define the library.
set( BUILD_SHARED TRUE CACHE BOOL "Disable to build static libraries." )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -masm=intel -Wall -Wextra -Wno-expansion-to-defined" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )
if(${BUILD_SHARED})
    message( STATUS "Building shared library." )
    add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
else()
    message( STATUS "Building static library." )
    add_library( ${PROJECT_NAME} STATIC ${SSCE_SRC} )
endif()
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SSCE_INC}" )
include_directories( "${SSCE_DIR_SRC}" )

## Define installation.
include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

## Uninstall.
if(NOT TARGET uninstall)
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
    add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
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
else( DOXYGEN_FOUND )
    message( "Doxygen need to be installed to generate the doxygen documentation" )
endif( DOXYGEN_FOUND )

## Test configuration.
enable_testing()
find_package( Threads )
define_test( "MODULE_STRING" "memswap" "multiconcat" )
define_test( "MODULE_STRUCTURES" "heapsort" )
