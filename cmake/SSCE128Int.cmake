include( CheckTypeSize )

function(add_128int)
    check_type_size( __int128 INT128_TYPE )
    if( ${HAVE_INT128_TYPE} )
        message( STATUS "Size of __int128: ${INT128_TYPE}" )
        add_compile_definitions( INT128_SUPPORTED "int128_t=__int128" "uint128_t=unsigned __int128" )
    endif()
endfunction()