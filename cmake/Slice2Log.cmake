#
# This file contains a set of macros used for generating 
# C++ source of log utilities from Slice files
#
# Author: Alex Makarenko
#

#
# Appends the new_bit to the original.
# If the original is not set, it will be set to the new_bit.
#
MACRO( APPEND original new_bit )

  IF    ( NOT ${original} )
    SET( ${original} ${new_bit} )
  ELSE  ( NOT ${original} )
    SET( ${original} ${${original}} ${new_bit} )
  ENDIF( NOT ${original} )

ENDMACRO( APPEND original new_bit )

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
MACRO( ORCA_GENERATE_SLICE2LOG_RULES generated_cpp_list generated_header_list )

    SET( slice_cpp_suffixes        .cpp )
    SET( slice_header_suffixes     .h  )
    SET( slice_suffixes            ${slice_cpp_suffixes} ${slice_header_suffixes} )
    SET( generator_command         ${ORCA_SLICE2LOG_COMMAND} )
    
    SET( proj_slice_src_dir        ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
    SET( proj_slice_bin_dir        ${PROJECT_BINARY_DIR}/src/interfaces/slice )

    SET( slice_module              ${PROJECT_NAME_LOWER} )
    SET( lib_namespace             ${PROJECT_NAME_LOWER}ifacelog )
    SET( proj_cpp_bin_dir          ${PROJECT_BINARY_DIR}/src/interfaces/cpp )
    
    # this auto-generated file will include all individual header files
    SET( global_header_file        ${lib_namespace}.h )
    SET( global_header_path        ${proj_cpp_bin_dir}/${lib_namespace}/${global_header_file} )

    # debian package splits off slice files into a different place
    IF( ICE_HOME MATCHES /usr )
        SET( ice_slice_dir /usr/share/slice )
        MESSAGE( STATUS "This is a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
    ELSE ( ICE_HOME MATCHES /usr )
        SET( ice_slice_dir ${ICE_HOME}/slice )
        MESSAGE( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
    ENDIF( ICE_HOME MATCHES /usr )

    # satellite projects need to include slice files from orca installation
    # NOTE: funky interaction between cmake and slice2cpp: cannot use "" around the slice_args!
    IF( ORCA_MOTHERSHIP )
        SET( slice_args -I${proj_slice_src_dir} -I${ice_slice_dir} --module ${slice_module} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
    ELSE ( ORCA_MOTHERSHIP )
        SET( orca_slice_dir ${ORCA_HOME}/share/orca/slice )
        SET( slice_args -I${proj_slice_src_dir} -I${ice_slice_dir} -I${orca_slice_dir} --module ${slice_module} --output-dir ${proj_cpp_bin_dir}/${lib_namespace} )
    ENDIF( ORCA_MOTHERSHIP )


  # debug
#   MESSAGE( STATUS "   slice sources    : " ${proj_slice_src_dir} )
#   MESSAGE( STATUS "   cpp destination  : " ${proj_cpp_bin_dir} )
#   MESSAGE( STATUS "ORCA_GENERATE_SLICE2LOG_RULES: ARGN: ${ARGN}" )
    
    #
    # First pass: Loop through the SLICE sources we were given, add the full path for dependencies
    #
    FOREACH( slice_source_basename ${ARGN} )
        APPEND( all_slice_depends "${proj_slice_src_dir}/${slice_module}/${slice_source_basename}" )
    ENDFOREACH( slice_source_basename ${ARGN} )
    
    #
    # Second pass: Loop through the SLICE sources we were given, add the CMake rules
    #
    SET( slice_source_counter 0 )
    FOREACH( slice_source_basename ${ARGN} )
        SET( slice_source "${proj_slice_src_dir}/${slice_module}/${slice_source_basename}" )
        #MESSAGE( STATUS "DEBUG: Dealing with ${slice_source_basename}")
    
        #
        # Add a custom cmake command to generate each type of output file: headers and source
        #
        FOREACH ( suffix ${slice_suffixes} )
        
            # OUTPUT is the target we're generating rules for.
            STRING( REGEX REPLACE "\\.ice" ${suffix} output_basename "${slice_source_basename}" )
            SET( output_fullname "${proj_cpp_bin_dir}/${lib_namespace}/${output_basename}" )
        
            #
            # Make each .h and .cpp file depend on _every_ slice source.  This means that if you 
            # change any .ice file everything will be recompiled. We want this because slice files
            # can include each other and if the included file has changed the downstream file needs
            # to processed again. Unfortunately CMake can't track dependencies between slice files
            # so it's safer to make them all depend on each other.
            #
            SET( all_depends ${all_slice_depends} )
        
            #
            # For .cpp files (not .h files) add extra dependencies.
            # Let CMake know that it has to generate all the .h/.cpp files in the order listed.
            # To enforce this, make each .cpp file depend on _all_ the previously-listed .h/.cpp files.
            #
            IF( ${suffix} STREQUAL ".cpp" )
                SET( all_depends ${all_depends} ${headers_so_far} ${bodies_so_far} )
                STRING( REGEX REPLACE "\\.cpp" ".h" associated_header "${output_fullname}" )
                APPEND( headers_so_far ${associated_header} )
                APPEND( bodies_so_far  ${output_fullname} )
            ENDIF( ${suffix} STREQUAL ".cpp" )
        
            #
            # Add this output to the list of generated files
            #
            IF( ${suffix} STREQUAL ".cpp" )
                APPEND( ${generated_cpp_list} ${output_basename} )
                SET( generated_file_type "source" )
            ELSE( ${suffix} STREQUAL ".cpp" )
                APPEND( ${generated_header_list} ${output_basename} )
                SET( generated_file_type "header" )
            ENDIF( ${suffix} STREQUAL ".cpp" )
        
            #
            # Add the command to generate file.xxx from file.ice
            # Note: when the 'output' is needed, the 'command' will be called with the 'args'
            #
#             MESSAGE( STATUS "DEBUG: Adding rule for generating ${output_basename} from ${slice_source_basename}" )
            ADD_CUSTOM_COMMAND(
                OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${output_basename}
                COMMAND ${generator_command} --log ${slice_args} ${slice_source}
                DEPENDS ${all_depends}
                COMMENT "-- Generating ${generated_file_type} file of log utilities from ${slice_source}"
                VERBATIM
                )

        ENDFOREACH ( suffix ${slice_suffixes} )
    
        MATH( EXPR slice_source_counter "${slice_source_counter} + 1" )
    
    ENDFOREACH( slice_source_basename )
    
    #
    # global header file
    #     
    INCLUDE( ${ORCA_CMAKE_DIR}/WriteGlobalHeaderFile.cmake )
    WRITE_GLOBAL_HEADER_FILE( ${global_header_path} ${lib_namespace} "${lib_namespace}_${lib_namespace}_H"  
            ${${generated_header_list}}  )

    APPEND( ${generated_header_list} ${global_header_file} )

    MESSAGE( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
    MESSAGE( STATUS "${generator_command} <source.ice> ${slice_args}" )

ENDMACRO( ORCA_GENERATE_SLICE2LOG_RULES generated_cpp_list generated_header_list )
