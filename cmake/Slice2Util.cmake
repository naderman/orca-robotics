#
# This file contains a set of macros used for generating 
# C++ source of utility functions from Slice files
#
# Author: Alex Makarenko
#

#
# Appends the new_bit to the original.
# If the original is not set, it will be set to the new_bit.
#
macro( APPEND original new_bit )

  IF    ( NOT ${original} )
    set( ${original} ${new_bit} )
  else( NOT ${original} )
    set( ${original} ${${original}} ${new_bit} )
  endif( NOT ${original} )

endmacro( APPEND original new_bit )

#
# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# ORCA_GENERATE_SLICE2UTIL_RULES( generated_cpp_list generated_header_list SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: generated_cpp_list and generated_header_list)
#
# Sets up dependencies: 
#  - Each .cpp and .h file depends on all the .ice files.
#  - Each .cpp file depends on all .h files associated with all the previously-listed slice sources.
#
macro( ORCA_GENERATE_SLICE2UTIL_RULES generated_cpp_list generated_header_list )

    set( slice_cpp_suffixes        .cpp )
    set( slice_header_suffixes     .h  )
    set( slice_suffixes            ${slice_cpp_suffixes} ${slice_header_suffixes} )
    set( generator_command         ${ORCA_SLICE2ORCA_COMMAND} )
    
    set( proj_slice_src_dir        ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
    set( proj_slice_bin_dir        ${PROJECT_BINARY_DIR}/src/interfaces/slice )

    set( slice_module              ${PROJECT_NAME_LOWER} )
    set( lib_namespace             ${PROJECT_NAME_LOWER}ifaceutil )
    set( proj_cpp_bin_dir          ${PROJECT_BINARY_DIR}/src/interfaces/cpp )
    
    # this auto-generated file will include all individual header files
    set( global_header_file        ${lib_namespace}.h )
    set( global_header_path        ${proj_cpp_bin_dir}/${lib_namespace}/${global_header_file} )

    # satellite projects need to include slice files from orca installation
    # NOTE: funky interaction between cmake and slice2cpp: cannot use "" around the slice_args!
    if( ORCA_MOTHERSHIP )
        set( slice_args -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} --module ${slice_module} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
    else( ORCA_MOTHERSHIP )
        set( orca_slice_dir ${ORCA_SLICE_DIR} )
        set( slice_args -I${proj_slice_src_dir} -I${ICE_SLICE_DIR} -I${orca_slice_dir} --module ${slice_module} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
    endif( ORCA_MOTHERSHIP )

    #
    # First pass: Loop through the SLICE sources we were given, add the full path for dependencies
    #
    foreach( slice_source_basename ${ARGN} )
        APPEND( all_slice_depends "${proj_slice_src_dir}/${slice_module}/${slice_source_basename}" )
    endforeach( slice_source_basename ${ARGN} )
    
    #
    # Second pass: Loop through the SLICE sources we were given, add the CMake rules
    #
    set( slice_source_counter 0 )
    foreach( slice_source_basename ${ARGN} )
        set( slice_source "${proj_slice_src_dir}/${slice_module}/${slice_source_basename}" )
        #message( STATUS "DEBUG: Dealing with ${slice_source_basename}")
    
        #
        # Add a custom cmake command to generate each type of output file: headers and source
        #
        foreach( suffix ${slice_suffixes} )
        
            # OUTPUT is the target we're generating rules for.
            string( REGEX REPLACE "\\.ice" ${suffix} output_basename "${slice_source_basename}" )
            set( output_fullname "${proj_cpp_bin_dir}/${lib_namespace}/${output_basename}" )
        
            #
            # Make each .h and .cpp file depend on _every_ slice source.  This means that if you 
            # change any .ice file everything will be recompiled. We want this because slice files
            # can include each other and if the included file has changed the downstream file needs
            # to processed again. Unfortunately CMake can't track dependencies between slice files
            # so it's safer to make them all depend on each other.
            #
            set( all_depends ${all_slice_depends} )
        
            #
            # For .cpp files (not .h files) add extra dependencies.
            # Let CMake know that it has to generate all the .h/.cpp files in the order listed.
            # To enforce this, make each .cpp file depend on _all_ the previously-listed .h/.cpp files.
            #
            if( ${suffix} STREQUAL ".cpp" )
                set( all_depends ${all_depends} ${headers_so_far} ${bodies_so_far} )
                string( REGEX REPLACE "\\.cpp" ".h" associated_header "${output_fullname}" )
                APPEND( headers_so_far ${associated_header} )
                APPEND( bodies_so_far  ${output_fullname} )
            endif( ${suffix} STREQUAL ".cpp" )
        
            #
            # Add this output to the list of generated files
            #
            if( ${suffix} STREQUAL ".cpp" )
                APPEND( ${generated_cpp_list} ${output_basename} )
                set( generated_file_type "source" )
            else( ${suffix} STREQUAL ".cpp" )
                APPEND( ${generated_header_list} ${output_basename} )
                set( generated_file_type "header" )
            endif( ${suffix} STREQUAL ".cpp" )
        
            #
            # Add the command to generate file.xxx from file.ice
            # Note: when the 'output' is needed, the 'command' will be called with the 'args'
            #
#             message( STATUS "DEBUG: Adding rule for generating ${output_basename} from ${slice_source_basename}" )
            add_custom_command(
                OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${output_basename}
                COMMAND ${generator_command} --util ${slice_args} ${slice_source}
                DEPENDS ${all_depends}
                COMMENT "-- Generating ${generated_file_type} file of utility functions from ${slice_source}"
                VERBATIM
                )

        endforeach( suffix ${slice_suffixes} )
    
        math( EXPR slice_source_counter "${slice_source_counter} + 1" )
    
    endforeach( slice_source_basename )
    
    #
    # global header file
    #     
    include( ${ORCA_CMAKE_DIR}/WriteGlobalHeaderFile.cmake )
    WRITE_GLOBAL_HEADER_file( ${global_header_path} ${lib_namespace} "${lib_namespace}_${lib_namespace}_H"  
            ${${generated_header_list}}  )

    APPEND( ${generated_header_list} ${global_header_file} )

    message( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
    message( STATUS "${generator_command} <source.ice> ${slice_args}" )

endmacro( ORCA_GENERATE_SLICE2UTIL_RULES generated_cpp_list generated_header_list )
