#
# This file contains a set of macros used for generating 
# Java source from Slice files
#
# Author: Alex Makarenko, based on slice2cpp_rules.cmake
#

set( SLICE2JAVA_COMMAND        ${ICE_HOME}/bin/slice2java${EXE_EXTENSION} )

set( SLICE_SOURCE_DIR          ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
set( SLICE_BINARY_DIR          ${PROJECT_BINARY_DIR}/src/interfaces/slice )
set( SLICE2JAVA_BINARY_DIR     ${PROJECT_BINARY_DIR}/src/interfaces/java )

# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
get_filename_component( INTERFACE_NAMESPACE ${CMAKE_CURRENT_SOURCE_DIR} NAME )

# note: satelite projects need to include slice files from orca installation
# note: compared to slice2cpp, slice2java automatically places generated files into 'namespace dir,
#       e.g. 'orca'. So the output dir is just 'java' not 'java/<namespace>'
if( DEFINED ORCA_HOME )
    set( SLICE_ARGS -I${SLICE_SOURCE_DIR} -I${ICE_HOME}/slice -I${ORCA_HOME}/slice --stream --output-dir ${SLICE2JAVA_BINARY_DIR} )
else( DEFINED ORCA_HOME )
    set( SLICE_ARGS -I${SLICE_SOURCE_DIR} -I${ICE_HOME}/slice --stream --output-dir ${SLICE2JAVA_BINARY_DIR} )
endif( DEFINED ORCA_HOME )


#
# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# GENERATE_SLICE2JAVA_RULES( GENERATED_CPP_LIST GENERATED_HEADER_LIST SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: GENERATED_CPP_LIST and GENERATED_HEADER_LIST)
#
# Sets up dependencies: 
#  - Each .cpp and .h file depends on all the .ice files.
#  - Each .cpp file depends on all .h files associated with all the previously-listed slice sources.
#
macro( GENERATE_SLICE2JAVA_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )

    #
    # Add the command to generate file.xxx from file.ice
    # Note: when the 'output' is needed, the 'command' will be called with the 'args'
    #
    message( STATUS "DEBUG: Adding rule for generating ${OUTPUT_BASENAME} from ${SLICE_SOURCE_BASENAME}" )
    add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/output.fake
        COMMAND ${SLICE2JAVA_COMMAND} 
        ARGS    "*.ice ${SLICE_ARGS}"
        DEPENDS ${SLICE_SOURCE_DIR}/${INTERFACE_NAMESPACE}/common.ice
        COMMENT "-- Generating .java files from all Slice sources"
    )

  #message( STATUS "DEBUG: GENERATED_CPP_LIST: ${${GENERATED_CPP_LIST}}")
  message( STATUS "Will generate java files from ${SLICE_SOURCE_COUNTER} Slice definitions using this command:" )
  message( STATUS "${SLICE2JAVA_COMMAND} <source.ice> ${SLICE_ARGS}" )

endmacro( GENERATE_SLICE2JAVA_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )
