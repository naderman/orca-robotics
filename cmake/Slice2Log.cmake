#
# This file contains a set of macros used for generating 
# C++ source of log functions from Slice files
#
# Author: Alex Makarenko
#

#
# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# ORCA_GENERATE_SLICE2LOG_RULES( generated_cpp_list generated_header_list SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: generated_cpp_list and generated_header_list)
#
# Sets up dependencies: 
#  - Each .cpp and .h file depends on all the .ice files.
#  - Each .cpp file depends on all .h files associated with all the previously-listed slice sources.
#
macro( ORCA_GENERATE_SLICE2LOG_RULES generated_cpp_list generated_header_list )

    # generator is used at make time, depend generator is used at cmake time.
    # the custom generator is not available at cmake time, we must use slice2cpp
    set( generator_command         ${ORCA_SLICE2ORCA_COMMAND} )
    set( depend_command            ${ICE_HOME}/bin/slice2cpp${EXE_EXTENSION} )
    set( slice_module              ${GBX_PROJECT_NAME_LOWER} )
    set( lib_namespace             ${GBX_PROJECT_NAME_LOWER}ifacelog )
    set( proj_slice_src_dir        ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
#     set( proj_slice_bin_dir        ${PROJECT_BINARY_DIR}/src/interfaces/slice )
    set( proj_cpp_bin_dir          ${PROJECT_BINARY_DIR}/src/interfaces/cpp )
    
    # NOTE: funky interaction between cmake variable and the Makefile generation: cannot use "" around the slice_args!
    if( ORCA_MOTHERSHIP )
        set( slice_args -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
        set( generator_args ${slice_args} --module ${slice_module} --log )
    else()
        # satellite projects need to include slice files from orca installation
        set( orca_slice_dir ${ORCA_SLICE_DIR} )
        set( slice_args -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} -I${orca_slice_dir} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
        set( generator_args ${slice_args} --module ${slice_module} --log )
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
            ARGS ${generator_args} ${slice_source}
            DEPENDS ${slice_depends}
            COMMENT "-- Generating header file of log functions from ${slice_source}"
            VERBATIM
        )

#         message( STATUS "DEBUG: Adding rule for generating source file from ${slice_source_basename}" )
        string( REGEX REPLACE "\\.ice" ".cpp" cpp_basename "${slice_source_basename}" )
        list( APPEND ${generated_cpp_list} ${cpp_basename} )
        # this command will be run at make time
        add_custom_command(
            OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${cpp_basename}
            COMMAND ${generator_command}
            ARGS ${generator_args} ${slice_source}
            DEPENDS ${slice_depends} ${header_basename} #${header_depends}
            COMMENT "-- Generating source file of log functions from ${slice_source}"
            VERBATIM
        )
    
        math( EXPR slice_source_counter "${slice_source_counter} + 1" )
    
    endforeach( slice_source_basename )
    
    # this message is useful for manual generation
    message( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
    message( STATUS "${generator_command} <source.ice> ${slice_args}" )

endmacro( ORCA_GENERATE_SLICE2LOG_RULES generated_cpp_list generated_header_list )
