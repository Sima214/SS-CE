# Initialize cmake.
cmake_minimum_required( VERSION 3.12 )
set( CMAKE_DISABLE_SOURCE_CHANGES ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )
if( NOT CMAKE_BUILD_TYPE )
	message( STATUS "Setting build type to 'Release' as none was specified." )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
	## Set the possible values of build type for cmake-gui
	SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" )
endif()

# Better defaults.
set( CMAKE_C_FLAGS_DEBUG "-g3 -O1" CACHE STRING "Flags used by the C compiler during debug builds." FORCE )
set( CMAKE_CXX_FLAGS_DEBUG "-g3 -O1" CACHE STRING "Flags used by the CXX compiler during debug builds." FORCE )

find_package( PythonInterp 3 REQUIRED )
include( cmake/TargetPlatform.cmake )
include( cmake/SSCEModules.cmake )
include( cmake/TargetArch.cmake )
include( cmake/SSCE128Int.cmake )
include( CheckCCompilerFlag )

## Configure project and settings.
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 1 )
set( PROJECT_VERSION_PATCH 2 )
set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}" )
set( SSCE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( SSCE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( SSCE_VERSION_PATCH ${PROJECT_VERSION_PATCH} )
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
declare_module( "MODULE_MATH_CRYPTO" "Cryptographic related algorithms." )
declare_module( "MODULE_CLOCK" "Enable the high accuracy clock module" )
declare_module( "MODULE_MEMORY" "Enable memory manipulation utilities" )
declare_module( "MODULE_STRING" "Enable strings util functions" "MODULE_MEMORY" )
declare_module( "MODULE_STRUCTURES" "Enable the data structures module" "MODULE_STRING" )
declare_module( "MODULE_LOGGER" "Enable the logger module" "MODULE_MEMORY" "MODULE_STRINGS" "MODULE_STRUCTURES" )
declare_module( "MODULE_AI" "Enable the AI module" "MODULE_MEMORY" "MODULE_STRUCTURES" )
declare_module( "MODULE_AI_SEARCH" "Enable the AI search module" "MODULE_AI" )
declare_module( "MODULE_AI_SEARCH_UNINFORMED" "Enable support for AI blind search algorithms." "MODULE_AI_SEARCH" )
declare_module( "MODULE_AI_SEARCH_INFORMED" "Enable support for AI heuristic search algorithms." "MODULE_AI_SEARCH" )

## Extra configuration.
set( MODULE_LOGGER_FILE TRUE CACHE BOOL "Enable storing log files to disk." )
set( MODULE_LOGGER_FILE_PREFIX "logs/${PROJECT_NAME}_" CACHE STRING "If logger files are enabled, this sets the path prefix of the file." )
set( MODULE_MEMORY_FALLOC_STACK_SIZE "8*1024" CACHE STRING "How much kibibytes to allocate initially for each thread local stack. Note that reallocations may not be possible, so choose a large size here." )
set( MODULE_MEMORY_GALLOC_OVERRIDE ${UNIX} CACHE BOOL "Whether or not to override global malloc/calloc/realloc/free symbols. You are on your own if enable this on Windows." )
if( ${MODULE_MEMORY_GALLOC_OVERRIDE} )
    set( MEMORY_GALLOC_OVERRIDE 1 )
endif()
set( MODULE_MEMORY_GALLOC_DUMP_OPTIONS "gamble" CACHE STRING "Options string used by galloc_dump_stats. For more information see malloc_stats_print." )
set( MODULE_STRUCTURES_HASHSET_INITIAL_SIZE "4096/(2*sizeof(void*))" CACHE STRING "How many buckets HashSet should allocate by default at initialization. This will also be the minimal amount of allocated buckets." )
set( MODULE_STRUCTURES_HASHSET_SHRINK_RATIO "0.5f" CACHE STRING "Default value for shrink ratio." )
set( MODULE_STRUCTURES_HASHSET_EXPAND_RATIO "1.0f" CACHE STRING "Default value for expand ratio." )
set( MODULE_STRUCTURES_HASHSET_RESIZE_FACTOR "2" CACHE STRING "How much to increase or decrease bucket count when shrinking or expanding." )
set( MODULE_STANDALONE FALSE CACHE BOOL "Try to create a binary which does not depend on external libs." )
if( ${MODULE_STANDALONE} )
    set( STANDALONE 1 )
endif()
set( MODULE_TESTING TRUE CACHE BOOL "Whether to enable tests. Disabling may fix some build issues." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_BINARY_DIR}/generated/Config.h" )
configure_file( "${PROJECT_SOURCE_DIR}/src/Modules.h.in" "${PROJECT_BINARY_DIR}/generated/Modules.h" )
include_directories( "${PROJECT_BINARY_DIR}/generated" )

## Module definitions.
include_directories( "${SSCE_DIR_SRC}/core" )
set( SSCE_SRC "${SSCE_DIR_SRC}/core/Lifecycle_${SSCE_PLT}.c;${SSCE_DIR_SRC}/core/Runtime_${SSCE_ARCH}.c;${SSCE_DIR_SRC}/core/Runtime.c" )
set( SSCE_INC "${PROJECT_BINARY_DIR}/generated/Modules.h;${PROJECT_BINARY_DIR}/generated/Config.h;${SSCE_DIR_SRC}/core/Runtime.h;${SSCE_DIR_SRC}/core/Runtime.hpp" )
set( SSCE_INC_DIRS ${CMAKE_CURRENT_BINARY_DIR} ${SSCE_DIR_SRC} )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
define_module( "MODULE_MATH" "PrimeGenerator.c" "BiggerNumbers.h;MinMax.h;MinMax.hpp;PrimeGenerator.h;PrimeGenerator.hpp" )
define_module( "MODULE_MATH_CRYPTO" "HashSpooky.c;HashXX.c" "Hash.h;Hash.hpp" )
define_module( "MODULE_CLOCK" "Clock_${SSCE_PLT}.c" "Clock.h;Clock.hpp" )
define_module( "MODULE_MEMORY" "Swap_${SSCE_ARCH}.c;GAlloc.c;FAlloc.c" "Memory.h;Memory.hpp;FAlloc.h;GAlloc.h;GAlloc.hpp" )
define_module( "MODULE_STRING" "SStrings_${SSCE_PLT}.c;SStrings.c" "SStrings.h;SStrings.hpp" )
define_module( "MODULE_STRUCTURES" "Heap.c;Sort.c;SortedArray.c;Dequeue.c;HashSet.c" "Interface.h;Interface.hpp;Sort.h;Sort.hpp;Heap.h;Heap.hpp;SortedArray.h;SortedArray.hpp;Dequeue.h;Dequeue.hpp;HashSet.h;HashSet.hpp" )
define_module( "MODULE_LOGGER" "Logger.c" "Logger.h;Logger.hpp" )
define_module( "MODULE_AI" "" "" )
define_module( "MODULE_AI_SEARCH" "" "SearchProblem.h;SearchProblem.hpp" )
define_module( "MODULE_AI_SEARCH_UNINFORMED" "BFS.c;DFS.c" "BFS.h;BFS.hpp;DFS.h;DFS.hpp" )
define_module( "MODULE_AI_SEARCH_INFORMED" "BestFirst.c" "BestFirst.h;BestFirst.hpp" )

# Prepare external libraries.
set( CMAKE_THREAD_PREFER_PTHREAD TRUE )
set( THREADS_PREFER_PTHREAD_FLAG TRUE )
find_package( Threads REQUIRED )

# Jemalloc module.
option( JEMALLOC_BUILD_STATIC "Build static libraries" OFF )
option( JEMALLOC_BUILD_SHARED "Build shared libraries" OFF )
option( JEMALLOC_CXX_DISABLE "Disable C++ integration" ON )
option( JEMALLOC_SMALLOCX_ENABLE "Enable experimental smallocx API" OFF )
if( ${MODULE_STANDALONE} )
    set( JEMALLOC_STATS_DISABLE ON CACHE BOOL "Disable statistics calculation/reporting" FORCE )
endif()
set(JEMALLOC_PREFIX "je_" CACHE STRING "Prefix to prepend to all public APIs")
add_subdirectory( jemalloc )
include_directories( "${CMAKE_BINARY_DIR}/jemalloc/include/jemalloc" )

# Check and add support for 128bit integers.
add_128int()

## Define the library.
message( STATUS "Building shared library." )
add_library( ${PROJECT_NAME} SHARED $<TARGET_OBJECTS:${JEMALLOC_MODULE_TARGET}> ${SSCE_SRC} )
if(UNIX AND NOT APPLE)
    target_link_libraries( ${PROJECT_NAME} rt )
endif()
target_link_libraries( ${PROJECT_NAME} Threads::Threads m ${CMAKE_DL_LIBS} )
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${SSCE_INC}" )

## Debug linking flags generated by cmake.
if( NOT TARGET verblink )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerbLink.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/VerbLink.cmake" IMMEDIATE @ONLY )
    add_custom_target( verblink COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/VerbLink.cmake )
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
    include( CheckIPOSupported )
    check_ipo_supported( RESULT ipo_supported OUTPUT ipo_error )
    if( ${ipo_supported} )
        message( STATUS "IPO/LTO is supported and enabled." )
        set_property( TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE )
    else()
        message(STATUS "IPO/LTO is not supported: <${ipo_error}>.")
    endif()
endif()
set( SSCE_FLAGS "${SSCE_FLAGS} -Wall -Wextra -Wno-expansion-to-defined -fvisibility=hidden" )
set( SSCE_FLAGS "${SSCE_FLAGS} -fexec-charset=UTF-8" )
set( SSCE_FLAGS "${SSCE_FLAGS} -masm=intel" )
set( SSCE_FLAGS "${SSCE_FLAGS} -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-memmov -fno-builtin-strlen -fno-builtin-memcmp" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SSCE_FLAGS}" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSCE_FLAGS}" )

## Define installation.
install( TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "bin"
        ARCHIVE DESTINATION "bin"
        LIBRARY DESTINATION "bin"
        PUBLIC_HEADER DESTINATION "include/${PROJECT_NAME}" )

## Uninstall script.
if( NOT TARGET uninstall )
    configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY )
    add_custom_target( uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake )
endif()

## Packaging.
set( CPACK_PACKAGE_NAME "SS-CE" )
set( CPACK_PACKAGE_VENDOR "Sima214" )
set( CPACK_PACKAGE_DESCRIPTION "Super simply: C/C++ extensions" )
set( CPACK_PACKAGE_HOMEPAGE_URL "https://sima214.me/ssce" )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" )
set( CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md" )
set( CPACK_PACKAGE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
set( CPACK_SOURCE_IGNORE_FILES "build" ".vscode" ".git" )
set( CPACK_PACKAGE_FILE_NAME "package" )
set( CPACK_SOURCE_PACKAGE_FILE_NAME "sources" )
set( CPACK_STRIP_FILES TRUE )
set( CPACK_GENERATOR "TBZ2" )
set( CPACK_SYSTEM_NAME ${SSCE_PLT} )
add_custom_target( 
    deploy
    ${PYTHON_EXECUTABLE}
    "${CMAKE_CURRENT_SOURCE_DIR}/upload.py"
    "${CMAKE_CURRENT_BINARY_DIR}/package.tar.bz2"
)
## This must come last.
include( CPack )

## Test configuration.
if( MODULE_TESTING )
    enable_testing()
    define_test( "MODULE_MATH_CRYPTO" "hash_spooky" )
    define_test( "MODULE_CLOCK" "timings" )
    define_test( "MODULE_MEMORY" "swap" "galloc" "falloc" )
    define_test( "MODULE_STRING" "concat" "puts" )
    define_test( "MODULE_STRUCTURES" "heapsort" "heap" "sorted_array" "dequeue" "hashset" )
    define_test( "MODULE_LOGGER" "core" )
    define_test( "MODULE_AI_SEARCH_UNINFORMED" "bfs" "dfs" )
    define_test( "MODULE_AI_SEARCH_INFORMED" "bestfirst" )
else()
    message( STATUS "Testing is not enabled!" )
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

## Export for use as submodule.
get_directory_property( submodule PARENT_DIRECTORY )
if( submodule )
    set( SSCE_MODULE_LIB ${PROJECT_NAME} PARENT_SCOPE )
    set( SSCE_MODULE_INCLUDE ${SSCE_INC_DIRS} PARENT_SCOPE )
endif()