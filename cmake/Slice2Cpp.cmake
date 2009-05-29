#
# This file contains a set of macros used for generating 
# C++ source from Slice files
#
# Author: Alex Brooks, Alex Makarenko
#

# ORCA_GENERATE_SLICE2CPP_RULES( generated_cpp_list generated_header_list SRC1 [SRC2 [SRC3...]] )
# 
# Generate rules for SLICE->C++ files generation, for each of the named slice source files.
#
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: generated_cpp_list and generated_header_list)
#
# Sets up dependencies: 
# - Each .h and .cpp file depends on the .ice files on which the source .ice file depends
# - Each .cpp file depends on the corresponding .h file.
#
# Features:
# - touch one of .ice files (e.g. rangescanner2d.ice) : only the dependent .h and .cpp file will be regenerated
#   and recompiled.
# - delete one of the generated .cpp files (e.g. rangescanner2d.cpp) : only it will be regenerated and recompiled.
# - delete one of the generated .h files (e.g. rangescanner2d.h) : only it and the associated .cpp file will be
#   regenerated, all .cpp files dependent on the header file will be recompiled. (Issue: the .cpp file does not
#   need to be regenerated. But I cannot seem to create a proper target for the header file by itself. To force
#   its regeneration, I had to set up a dependency of the .cpp file on the header file.)
macro( ORCA_GENERATE_SLICE2CPP_RULES generated_cpp_list generated_header_list )

    set( generator_command         ${ICE_HOME}/bin/slice2cpp${EXE_EXTENSION} )
    set( depend_command            ${generator_command} )
    set( slice_module              ${GBX_PROJECT_NAME_LOWER} )
    set( proj_slice_src_dir        ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
#     set( proj_slice_bin_dir        ${PROJECT_BINARY_DIR}/src/interfaces/slice )
    set( slice2cpp_binary_dir      ${PROJECT_BINARY_DIR}/src/interfaces/cpp )

    # NOTE: funky interaction between cmake variable and the Makefile generation: cannot use "" around the slice_args!
    if( ORCA_MOTHERSHIP )
        set( slice_args ${SLICE_PROJECT_ARGS} -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} --include-dir ${slice_module} --stream --output-dir ${slice2cpp_binary_dir}/${slice_module} )
    else()
        # satellite projects need to include slice files from orca installation
        set( orca_slice_dir ${ORCA_SLICE_DIR} )
        set( slice_args ${SLICE_PROJECT_ARGS} -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} -I${orca_slice_dir} --include-dir ${slice_module} --stream --output-dir ${slice2cpp_binary_dir}/${slice_module} )
    endif()

    #
    # Loop through all SLICE sources we were given, add the CMake rules
    #
    set( slice_source_counter 0 )
    foreach( slice_source_basename ${ARGN} )
        set( slice_source "${proj_slice_src_dir}/${slice_module}/${slice_source_basename}" )
        #message( STATUS "DEBUG: Dealing with ${slice_source_basename}")
    
        #
        # figure out dependencies of this source.ice file
        # only slice2cpp knows how to do it.
        #
        set( depend_file ${CMAKE_CURRENT_BINARY_DIR}/depend.txt )
        # this command is run at cmake time!
        execute_process( 
            COMMAND ${depend_command} ${slice_args} ${slice_source} --depend
            # can pipe the output straight to a cmake variable, but don't know how to parse it to a list.
#             OUTPUT_VARIABLE slice_depends
            OUTPUT_FILE ${depend_file} )
        # parse the file into a list of strings
        file( STRINGS ${depend_file} slice_depends_dirty )
        # IMPORTANT: this magic line is required, no idea why.
        # without it, 'slice_depends' is not a list, has only one element.
        set( slice_depends_dirty ${slice_depends_dirty} )
        # the first element is the generated file, we remove it.
        list( REMOVE_AT slice_depends_dirty 0 )
        # the rest are the .ice file dependencies. there's at least one: the actual source .ice file.
        # all dependencies end up having a leading "\ "
        # if we don't remove them, add_custom_command() will interpret them as relative commands
        set( slice_depends "" )
        foreach( slice_depend ${slice_depends_dirty} )
#             message( STATUS: "${output_basename} depends on ${slice_depend}" )
            string( LENGTH ${slice_depend} slice_depend_length ) 
            math( EXPR slice_depend_length_1 "${slice_depend_length}-1" )
            string( SUBSTRING ${slice_depend} 1 ${slice_depend_length_1} slice_depend )
            string( STRIP ${slice_depend} slice_depend )
            list( APPEND slice_depends ${slice_depend} )
        endforeach( slice_depend ${slice_depends_dirty} )

        # alexm: I'm not sure we need this. this dependency will be enforce automatically through .ice files???
        # in addition, the object files will also depend on the header files
        # the dependencies are the same as the .ice files, we just change extensions.
#         set( header_depends "" )
#         foreach( slice_depend ${slice_depends} )
# #             message( STATUS: "${output_basename} depends on ${slice_depend}" )
#             string( REGEX REPLACE "\\.ice" ".h" header_depend "${slice_depend}" )
#             list( APPEND header_depends ${header_depend} )
#         endforeach( slice_depend ${slice_depends} )

        #
        # Add the command to generate file.h and file.cpp from file.ice
        # Note: when the 'output' is needed, the 'command' will be called with the 'args'
        #
#         message( STATUS "DEBUG: Adding rule for generating header file from ${slice_source_basename}" )
        string( REGEX REPLACE "\\.ice" ".h" header_basename "${slice_source_basename}" )
        list( APPEND ${generated_header_list} ${header_basename} )
        # this command will be run at make time
        add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${header_basename}
            COMMAND ${generator_command}
            ARGS ${slice_args} ${slice_source}
            DEPENDS ${slice_depends}
            COMMENT "-- Generating header file from ${slice_source}"
            VERBATIM
        )

#         message( STATUS "DEBUG: Adding rule for generating source file from ${slice_source_basename}" )
        string( REGEX REPLACE "\\.ice" ".cpp" cpp_basename "${slice_source_basename}" )
        list( APPEND ${generated_cpp_list} ${cpp_basename} )
        # this command will be run at make time
        add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${cpp_basename}
            COMMAND ${generator_command}
            ARGS ${slice_args} ${slice_source}
            DEPENDS ${slice_depends} ${header_basename} #${header_depends}
            COMMENT "-- Generating source file from ${slice_source}"
            VERBATIM
        )
    
        math( EXPR slice_source_counter "${slice_source_counter} + 1" )

    endforeach( slice_source_basename )
    
    #   message( STATUS "DEBUG: generated_cpp_list: ${${generated_cpp_list}}")
    #   message( STATUS "DEBUG: generated_header_list: ${${generated_header_list}}")
    
    # this message is useful for manual generation
    message( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
    message( STATUS "${generator_command} <source.ice> ${slice_args}" )

endmacro( ORCA_GENERATE_SLICE2CPP_RULES generated_cpp_list generated_header_list )
