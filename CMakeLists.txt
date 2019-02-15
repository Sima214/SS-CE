cmake_minimum_required( VERSION 3.6 )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
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
        string( TOLOWER ${namestripped} mn )
        set( folder "${SSCE_DIR_SRC}/${mn}" )
        include_directories( ${folder} )
        foreach(cc ${src})
            list( APPEND srclist "${folder}/${cc}" )
        endforeach(cc ${src})
        set( SSCE_SRC ${SSCE_SRC} ${srclist} PARENT_SCOPE )
        foreach(ch ${inc})
            list( APPEND headerlist "${folder}/${ch}" )
        endforeach(ch ${inc})
        set( SSCE_INC ${SSCE_INC} ${headerlist} PARENT_SCOPE )
        ## Add lint targets for each module.
        if( EXISTS ${BUILD_LINT} AND srclist )
            list( JOIN srclist " " srcliststr )
            set( LINT_INCLUDE "${LINT_INCLUDE} -I${folder}" )
            ## Update includes for next modules.
            set( LINT_INCLUDE ${LINT_INCLUDE} PARENT_SCOPE )
            ## Generate the final arguments.
            separate_arguments( arguments UNIX_COMMAND "${LINT_OPTIONS} -o ${CMAKE_CURRENT_BINARY_DIR}/lint/${mn}.html -p ${CMAKE_CURRENT_BINARY_DIR} ${srcliststr} -- ${CMAKE_C_FLAGS} ${LINT_INCLUDE} ${LINT_QUIRKS} || true" )
            add_custom_command( OUTPUT "lint/${mn}.html" COMMAND ${BUILD_LINT} ${arguments} WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lint" DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/lint/.oclint" ${srclist} VERBATIM )
            if( DEFINED LINT_LIST )
                set( LINT_LIST "${LINT_LIST}\;lint/${mn}.html" PARENT_SCOPE)
            else( DEFINED LINT_LIST )
                set( LINT_LIST "lint/${mn}.html" PARENT_SCOPE)
            endif( DEFINED LINT_LIST )
        endif( EXISTS ${BUILD_LINT} AND srclist )
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

## Configure compiler.
include_directories( "${SSCE_DIR_SRC}" )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-expansion-to-defined -fvisibility=hidden" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=UTF-8" ) 
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -masm=intel" ) 
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )

## Module selection.
declare_module( "MODULE_MATH" "Random math utilities." )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module." )
declare_module( "MODULE_THREADS" "Enable posix thread support." )
declare_module( "MODULE_STRING" "Enable strings util functions." )
declare_module( "MODULE_LOGGER" "Enable the logger module." "MODULE_STRINGS" "MODULE_THREADS" )
declare_module( "MODULE_ARGPARSE" "Enable the argument parser module." )
declare_module( "MODULE_STRUCTURES" "Enable the data structures module." "MODULE_STRING" )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_SOURCE_DIR}/src/Config.h" )
configure_file( "${PROJECT_SOURCE_DIR}/src/Modules.h.in" "${PROJECT_SOURCE_DIR}/src/Modules.h" )

## Prepare Linter.
find_program( BUILD_LINT NAMES oclint DOC "The path to the C/C++ linter." )
if( NOT EXISTS ${BUILD_LINT} )
    ## Required linter is not installed.
    message( WARNING "oclint is required for linting support." )
else()
    ## Generate compiler flags for linter.
    set( LINT_OPTIONS "-no-analytics -enable-clang-static-analyzer -report-type=html" )
    set( LINT_INCLUDE "-I/usr/lib/clang/5.0.1/include -I/usr/include -I/usr/local/include" )
    ## Override code to avoid unimplemented things in the old version of clang oclint uses.
    set( LINT_QUIRKS "-D__m128i_u=__m128i -D__m256i_u=__m256i -Dcpu_init=rand" )
    ## Add any more include directories.
    get_property( include DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES )
    foreach(dir ${include})
        set( LINT_INCLUDE "${LINT_INCLUDE} -I${dir}" )
    endforeach()
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
if( ${BUILD_SHARED} )
    message( STATUS "Building shared library." )
    add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
else()
    message( STATUS "Building static library." )
    add_library( ${PROJECT_NAME} STATIC ${SSCE_SRC} )
endif()
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SSCE_INC}" )

## Define installation.
include( GNUInstallDirs )
install( TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

## Uninstall.
if( NOT TARGET uninstall )
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
    message( WARNING "Doxygen need to be installed to generate the doxygen documentation" )
endif( DOXYGEN_FOUND )

## Test configuration.
enable_testing()
find_package( Threads )
define_test( "MODULE_CLOCK" "timings" )
define_test( "MODULE_STRING" "memswap" "multiconcat" )
define_test( "MODULE_STRUCTURES" "heapsort" "heap" )

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

if( EXISTS ${BUILD_LINT} )
    ## Add linter make targets and copy config file.
    file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lint" )
    add_custom_target( lint DEPENDS ${LINT_LIST} )
    configure_file("${PROJECT_SOURCE_DIR}/.oclint" "${CMAKE_CURRENT_BINARY_DIR}/lint/.oclint" COPYONLY)
endif( EXISTS ${BUILD_LINT} )
