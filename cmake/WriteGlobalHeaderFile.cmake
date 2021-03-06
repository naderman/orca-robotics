#
# writes file ${global_header_file} which includes all header files in the list assuming that
# they are located in the specified directory.
#
macro( ORCA_WRITE_GLOBAL_HEADER_FILE global_header_file header_file_dir define_name )

  if ( NOT EXISTS ${global_header_file} )
    message( STATUS "Generating global header file ${global_header_file}" )
    write_file( ${global_header_file} "/*" ) # <-- not appending!
    write_file( ${global_header_file} " * Auto-generated by CMake." APPEND )
    write_file( ${global_header_file} " * Do not modify or commit to source repository." APPEND )
    write_file( ${global_header_file} " */" APPEND )
    write_file( ${global_header_file} "" APPEND )
    write_file( ${global_header_file} "#ifndef ${define_name}" APPEND )
    write_file( ${global_header_file} "#define ${define_name}" APPEND )
    write_file( ${global_header_file} "" APPEND )

    foreach( header_file ${ARGN} )
        write_file( ${global_header_file} "#include <${header_file_dir}/${header_file}>" APPEND )
    endforeach( header_file ${ARGN} )

    write_file( ${global_header_file} "" APPEND )
    write_file( ${global_header_file} "#endif" APPEND )
  endif ( NOT EXISTS ${global_header_file} )

    set_source_files_properties( ${global_header_file} PROPERTIES GENERATED TRUE )

endmacro( ORCA_WRITE_GLOBAL_HEADER_FILE global_header_file header_file_dir define_name )
