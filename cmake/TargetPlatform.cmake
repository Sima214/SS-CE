function(target_platform output_plt output_lib)
    if( WIN32 )
        message( STATUS "Compiling for windows." )
        set( ${output_plt} "win" PARENT_SCOPE )
        set( ${output_lib} "dll" PARENT_SCOPE )
        add_definitions( "-DWIN32_LEAN_AND_MEAN" )
        add_definitions( "-DUNICODE" )
        add_definitions( "-D__USE_MINGW_ANSI_STDIO" )
        add_definitions( "-DLINK_PE" )
    elseif( UNIX AND NOT APPLE )
        message( STATUS "Compiling for linux." )
        set( ${output_plt} "linux" PARENT_SCOPE )
        set( ${output_lib} "so" PARENT_SCOPE )
        add_definitions( "-D_GNU_SOURCE" )
        add_definitions( "-DLINK_ELF" )
    elseif( APPLE )
        message( STATUS "Compiling for macos." )
        set( ${output_plt} "mac" PARENT_SCOPE )
        set( ${output_lib} "dylib" PARENT_SCOPE )
        add_definitions( "-DLINK_MACHO" )
    else()
        message( FATAL_ERROR "Unsupported OS!!!" )
    endif()
endfunction()