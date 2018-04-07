cmake_minimum_required( VERSION 3.6 )
if( NOT CMAKE_BUILD_TYPE )
	message( STATUS "Setting build type to 'Release' as none was specified." )
	set( CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE )
	## Set the possible values of build type for cmake-gui
	SET_PROPERTY( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" )
endif()
function(handle_module modname src inc)
    if(${${modname}})
        message( STATUS "${modname}:\tON" )
        set( SSCE_SRC "${SSCE_SRC}\;${src}" PARENT_SCOPE )
        set( SSCE_INC "${SSCE_INC}\;${inc}" PARENT_SCOPE )
        file( APPEND "${PROJECT_SOURCE_DIR}/src/Modules.h" "#define ${modname}\n" )
    else(${${modname}})
        message( STATUS "${modname}:\tOFF" )
    endif(${${modname}})
endfunction()

## Configure project and settings.
set( PROJECT_VERSION_MAJOR 0 )
set( PROJECT_VERSION_MINOR 1 )
set( PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}" )
set( SSCE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR} )
set( SSCE_VERSION_MINOR ${PROJECT_VERSION_MINOR} )
set( SSCE_VERSION ${PROJECT_VERSION} )
set( PROJECT_NAME "ssce")
project( ${PROJECT_NAME} VERSION ${SSCE_VERSION} DESCRIPTION "A random C/C++ library." )
set( SSCE_DIR_SRC "${PROJECT_SOURCE_DIR}/src" )

## Module selection.
set( MODULE_MATH TRUE CACHE BOOL "Random math utilities." )
set( MODULE_CLOCK TRUE CACHE BOOL "Enable the high accuracy clock module." )
set( MODULE_THREADS TRUE CACHE BOOL "Enable thread cross-platform support." )
set( MODULE_STRINGS TRUE CACHE BOOL "Enable strings util functions." )
set( MODULE_LOGGER TRUE CACHE BOOL "Enable the logger module. Requires the strings and thread modules." )

## Extra configuration.
set( MODULE_LOGGER_LOGFILE TRUE CACHE BOOL "Enable storing log files to disk." )
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
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h\;${SSCE_DIR_SRC}/Mkdir.h\;${SSCE_DIR_SRC}/Modules.h" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
handle_module( "MODULE_MATH" "" "${SSCE_DIR_SRC}/math/MinMax.h\;${SSCE_DIR_SRC}/math/MinMax.hpp")
handle_module( "MODULE_CLOCK" "${SSCE_DIR_SRC}/clock/Clock_${SSCE_PLT}.c" "${SSCE_DIR_SRC}/clock/Clock.h\;${SSCE_DIR_SRC}/clock/Clock.hpp")
handle_module( "MODULE_THREADS" "" "${SSCE_DIR_SRC}/thread/Threads.h\;${SSCE_DIR_SRC}/thread/Threads.hpp")
handle_module( "MODULE_STRINGS" "${SSCE_DIR_SRC}/string/Strings_${SSCE_PLT}.c;${SSCE_DIR_SRC}/string/Strings.c" "${SSCE_DIR_SRC}/string/Strings.h\;${SSCE_DIR_SRC}/string/Strings.hpp")
handle_module( "MODULE_LOGGER" "${SSCE_DIR_SRC}/logger/Logger.c" "${SSCE_DIR_SRC}/logger/Logger.h\;${SSCE_DIR_SRC}/logger/Logger.hpp")
## More modules go here!!!

## Define the library.
message( STATUS "Building shared library." )
add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${SSCE_INC} )
target_include_directories( ${PROJECT_NAME} PRIVATE "${SSCE_DIR_SRC}" )

## Define installation.
include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)