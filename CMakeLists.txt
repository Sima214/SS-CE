cmake_minimum_required( VERSION 3.9.4 )
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )
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
            target_link_libraries( "test_${subname}" ${JEMALLOC_CFLAGS} ${JEMALLOC_LIBRARIES} Threads::Threads ${PROJECT_NAME} )
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
    set( DYNLIB_EXT "dll" )
    add_definitions( "-DWIN32_LEAN_AND_MEAN" )
    add_definitions( "-DUNICODE" )
    add_definitions( "-DLINK_PE" )
elseif( UNIX AND NOT APPLE )
    message( "Compiling for linux." )
    set( SSCE_PLT "linux" )
    set( DYNLIB_EXT "so" )
    add_definitions( "-D_GNU_SOURCE" )
    add_definitions( "-DLINK_ELF" )
elseif( APPLE )
    message( "Compiling for macos." )
    set( SSCE_PLT "mac" )
    set( DYNLIB_EXT "dylib" )
    add_definitions( "-DLINK_MACHO" )
else()
    message( FATAL_ERROR "Unsupported os!!!" )
endif()

# Architecture detection.
include( cmake/TargetArch.cmake )
target_architecture( SSCE_ARCH )
message( "Compiling for ${SSCE_ARCH}." )

# Prepare external libraries.
set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )
find_package( Threads REQUIRED )
find_package( PkgConfig )
include( ExternalProject )
set( SYSTEM_JEMALLOC FALSE CACHE BOOL "Link against system jemalloc library." )
if( ${SYSTEM_JEMALLOC} )
    if( WIN32 )
        # If you manage to make jemalloc on windows, then good for you.
        # I cannot, so just dump the jemalloc installation in <build>/jemalloc.
        add_library( jemalloc SHARED IMPORTED )
        set_target_properties( jemalloc PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/jemalloc/lib/jemalloc.${DYNLIB_EXT}" )
        include_directories( "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc" )
        set( JEMALLOC_LIBRARIES jemalloc )
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
                         CONFIGURE_COMMAND sh ./configure --prefix=${CMAKE_BINARY_DIR}/jemalloc
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

## Module selection.
declare_module( "MODULE_MATH" "Enable extra math functions" )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module" )
declare_module( "MODULE_MEMORY" "Enable memory manipulation utilities" )
declare_module( "MODULE_STRING" "Enable strings util functions" "MODULE_MEMORY" )
declare_module( "MODULE_STRUCTURES" "Enable the data structures module" "MODULE_STRING" )
declare_module( "MODULE_LOGGER" "Enable the logger module" "MODULE_MEMORY" "MODULE_STRINGS" "MODULE_STRUCTURES" )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
set( MODULE_LOGGER_FILE_PREFIX "logs/ssce_" CACHE STRING "If logger files are enabled, this sets the path prefix of the file." )
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
set( SSCE_SRC "${SSCE_DIR_SRC}/core/Lifecycle_${SSCE_PLT}.c" )
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h;${SSCE_DIR_SRC}/Modules.h;${SSCE_DIR_SRC}/Config.h" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
define_module( "MODULE_MATH" "" "MinMax.h;MinMax.hpp" )
define_module( "MODULE_CLOCK" "Clock_${SSCE_PLT}.c" "Clock.h;Clock.hpp" )
define_module( "MODULE_MEMORY" "Swap_${SSCE_ARCH}.c" "Memory.h;Memory.hpp" )
define_module( "MODULE_STRING" "Strings_${SSCE_PLT}.c;Strings.c" "Strings.h;Strings.hpp" )
define_module( "MODULE_STRUCTURES" "Heap.c;Sort.c" "Interface.h;Sort.h;Sort.hpp;Heap.h;Heap.hpp" )
define_module( "MODULE_LOGGER" "Logger.c" "Logger.h;Logger.hpp" )

## End of modules definitions.

## Define the library.
set( BUILD_SHARED TRUE CACHE BOOL "Disable to build static libraries." )
if( ${BUILD_SHARED} )
    message( STATUS "Building shared library." )
    add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
    target_link_libraries( ${PROJECT_NAME} ${JEMALLOC_CFLAGS} ${JEMALLOC_LIBRARIES} Threads::Threads )
else()
    message( STATUS "Building static library." )
    add_definitions( "-DLINK_STATIC" )
    add_library( ${PROJECT_NAME} STATIC ${SSCE_SRC} )
    target_link_libraries( ${PROJECT_NAME} ${JEMALLOC_CFLAGS} ${JEMALLOC_LIBRARIES} Threads::Threads )
    set_property( ${PROJECT_NAME} POSITION_INDEPENDENT_CODE TRUE )
endif()
if( NOT ${SYSTEM_JEMALLOC} )
    add_dependencies( ${PROJECT_NAME} jemalloc-build )
endif()
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SSCE_INC}" )

## Configure compiler.
include_directories( "${SSCE_DIR_SRC}" )
set( CMAKE_C_STANDARD 99 )
set( CMAKE_CXX_STANDARD 11 )
include( CheckCCompilerFlag )
## Optimization flags.
if( CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" )
    message( "Debug build detected, extra optimization flags are not enabled." )
else()
    check_c_compiler_flag( "-fno-plt" noplt )
    if( ${noplt} )
        target_compile_options( ${PROJECT_NAME} PRIVATE "-fno-plt" )
        message( STATUS "-fno-plt is supported and enabled for the library." )
    else( ${noplt} )
        message( STATUS "-fno-plt is not supported." )
    endif( ${noplt} )
    include( CheckIPOSupported )
    check_ipo_supported( RESULT ipo_supported OUTPUT ipo_error )
    if( ${ipo_supported} )
        message( STATUS "IPO/LTO is supported and enabled." )
        set_property( TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE )
    else( ${ipo_supported} )
        message(STATUS "IPO/LTO is not supported: <${ipo_error}>.")
    endif( ${ipo_supported} )
endif()
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-expansion-to-defined -fvisibility=hidden" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexec-charset=UTF-8" ) 
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -masm=intel" ) 
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )

## Define installation.
include( GNUInstallDirs )
install( TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install( FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig )
if( NOT ${SYSTEM_JEMALLOC} OR WIN32 )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CopyJemalloc.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/CopyJemalloc.cmake" IMMEDIATE @ONLY )
    install( SCRIPT "${CMAKE_BINARY_DIR}/CopyJemalloc.cmake" )
    install( FILES "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc/jemalloc.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
endif()

## Uninstall.
if( NOT TARGET uninstall )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
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
define_test( "MODULE_CLOCK" "timings" )
define_test( "MODULE_MEMORY" "swap" "jemalloc" )
define_test( "MODULE_STRING" "concat" "puts" )
define_test( "MODULE_STRUCTURES" "heapsort" "heap" )
define_test( "MODULE_LOGGER" "core" )

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
