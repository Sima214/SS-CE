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
## Module settings.
set( MODULE_CLOCK TRUE CACHE BOOL "Enable the high accuracy clock module." )

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
set( SSCE_SRC "" )
set( SSCE_INC "" )
message( STATUS "######################################################" )
message( STATUS "Configuring modules..." )
message( STATUS "######################################################" )
handle_module( "Clock" ${MODULE_CLOCK} "${SSCE_DIR_SRC}/clock/Clock.c" "${SSCE_DIR_SRC}/clock/Clock.h\;${SSCE_DIR_SRC}/clock/Clock.hpp")
## More modules go here!!!

## Define the library.
add_library( ${PROJECT_NAME} SHARED ${SSCE_SRC} )
set_target_properties( ${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${SSCE_INC} )

## Define installation.
include(GNUInstallDirs)
install( TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
configure_file( "${PROJECT_NAME}.pc.in" "${PROJECT_NAME}.pc" @ONLY )
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)