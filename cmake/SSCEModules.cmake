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
            target_link_libraries( "test_${subname}" ${PROJECT_NAME} )
            target_compile_options( "test_${subname}" PRIVATE "-fno-builtin" )
            add_test( NAME ${subname} COMMAND "test_${subname}" )
        endforeach(d)
    endif(${${modname}})
endfunction()