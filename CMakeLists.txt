cmake_minimum_required( VERSION 3.6 )
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
## Config
set( MODULE_LOGGER_LOGFILE TRUE CACHE BOOL "Enable storing log files to disk." )
configure_file( "${PROJECT_SOURCE_DIR}/src/Config.h.in" "${PROJECT_SOURCE_DIR}/src/Config.h" )
## Module settings.
set( MODULE_MATH TRUE CACHE BOOL "Random math utilities." )
set( MODULE_CLOCK TRUE CACHE BOOL "Enable the high accuracy clock module." )
set( MODULE_THREADS TRUE CACHE BOOL "Enable thread cross-platform support." )
set( MODULE_STRINGS TRUE CACHE BOOL "Enable strings util functions." )
set( MODULE_LOGGER TRUE CACHE BOOL "Enable the logger module. Requires the strings and thread modules." )

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

## Modules.
function(handle_module modname flag src inc)
    if(flag)
        message( STATUS "${modname}:\tON" )
        set( SSCE_SRC "${SSCE_SRC}\;${src}" PARENT_SCOPE )
        set( SSCE_INC "${SSCE_INC}\;${inc}" PARENT_SCOPE )
    else(flag)
        message( STATUS "${modname}:\tOFF" )
    endif(flag)
endfunction()
set( SSCE_SRC "${SSCE_DIR_SRC}/Lifecycle_${SSCE_PLT}.c" )
set( SSCE_INC "${SSCE_DIR_SRC}/Macros.h\;${SSCE_DIR_SRC}/Mkdir.h" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
handle_module( "Math" ${MODULE_MATH} "" "${SSCE_DIR_SRC}/math/MinMax.h\;${SSCE_DIR_SRC}/math/MinMax.hpp")
handle_module( "Clock" ${MODULE_CLOCK} "${SSCE_DIR_SRC}/clock/Clock_${SSCE_PLT}.c" "${SSCE_DIR_SRC}/clock/Clock.h\;${SSCE_DIR_SRC}/clock/Clock.hpp")
handle_module( "Threads" ${MODULE_THREADS} "" "${SSCE_DIR_SRC}/thread/Threads.h\;${SSCE_DIR_SRC}/thread/Threads.hpp")
handle_module( "Strings" ${MODULE_STRINGS} "${SSCE_DIR_SRC}/string/Strings_${SSCE_PLT}.c;${SSCE_DIR_SRC}/string/Strings.c" "${SSCE_DIR_SRC}/string/Strings.h\;${SSCE_DIR_SRC}/string/Strings.hpp")
handle_module( "Logger" ${MODULE_LOGGER} "${SSCE_DIR_SRC}/logger/Logger.c" "${SSCE_DIR_SRC}/logger/Logger.h\;${SSCE_DIR_SRC}/logger/Logger.hpp")
## More modules go here!!!

## Define the library.
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