#
# This file contains a set of macros used for generating 
# C++ source from Slice files
#
# Author: Alex Brooks
#

SET ( SLICE_CPP_SUFFIXES        .cpp )
SET ( SLICE_HEADER_SUFFIXES     .h  )
SET ( SLICE_SUFFIXES            ${SLICE_CPP_SUFFIXES} ${SLICE_HEADER_SUFFIXES} )
SET ( SLICE2CPP_COMMAND         ${ICE_HOME}/bin/slice2cpp${EXE_EXTENSION} )

SET ( SLICE_SOURCE_DIR          ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
SET ( SLICE_BINARY_DIR          ${PROJECT_BINARY_DIR}/src/interfaces/slice )
SET ( SLICE2CPP_BINARY_DIR      ${PROJECT_BINARY_DIR}/src/interfaces/cpp )

# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
GET_FILENAME_COMPONENT( INTERFACE_NAMESPACE ${CMAKE_CURRENT_SOURCE_DIR} NAME )

# satelite projects need to include slice files from orca installation
IF ( DEFINED ORCA2_HOME )
    SET ( SLICE_ARGS ${SLICE_PROJECT_ARGS} -I${SLICE_SOURCE_DIR} -I${ICE_SLICE_HOME}/slice -I${ORCA2_HOME}/slice --stream --output-dir ${SLICE2CPP_BINARY_DIR}/${INTERFACE_NAMESPACE} )
ELSE ( DEFINED ORCA2_HOME )
    SET ( SLICE_ARGS ${SLICE_PROJECT_ARGS} -I${SLICE_SOURCE_DIR} -I${ICE_SLICE_HOME}/slice --stream --output-dir ${SLICE2CPP_BINARY_DIR}/${INTERFACE_NAMESPACE} )
ENDIF ( DEFINED ORCA2_HOME )


#
# Appends the new_bit to the original.
# If the original is not set, it will be set to the new_bit.
#
MACRO( APPEND ORIGINAL NEW_BIT )

  IF    ( NOT ${ORIGINAL} )
    SET( ${ORIGINAL} ${NEW_BIT} )
  ELSE  ( NOT ${ORIGINAL} )
    SET( ${ORIGINAL} ${${ORIGINAL}} ${NEW_BIT} )
  ENDIF ( NOT ${ORIGINAL} )

ENDMACRO( APPEND ORIGINAL NEW_BIT )

#
# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# GENERATE_SLICE2CPP_RULES( GENERATED_CPP_LIST GENERATED_HEADER_LIST SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: GENERATED_CPP_LIST and GENERATED_HEADER_LIST)
#
# Sets up dependencies: 
#  - Each .cpp and .h file depends on all the .ice files.
#  - Each .cpp file depends on all .h files associated with all the previously-listed slice sources.
#
MACRO ( GENERATE_SLICE2CPP_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )

  # debug
#   MESSAGE ( STATUS "   slice sources    : " ${SLICE_SOURCE_DIR} )
#   MESSAGE ( STATUS "   cpp destination  : " ${SLICE2CPP_BINARY_DIR} )
#   MESSAGE ( STATUS "GENERATE_SLICE2CPP_RULES: ARGN: ${ARGN}" )

  #
  # First pass: Loop through the SLICE sources we were given, add the full path for dependencies
  #
  FOREACH( SLICE_SOURCE_BASENAME ${ARGN} )
    APPEND( GEN_SLICE_RULES_SLICE_DEPENDS "${SLICE_SOURCE_DIR}/${INTERFACE_NAMESPACE}/${SLICE_SOURCE_BASENAME}" )
  ENDFOREACH( SLICE_SOURCE_BASENAME ${ARGN} )

  #
  # Second pass: Loop through the SLICE sources we were given, add the CMake rules
  #
  SET ( SLICE_SOURCE_COUNTER 0 )
  FOREACH( SLICE_SOURCE_BASENAME ${ARGN} )
    SET( SLICE_SOURCE "${SLICE_SOURCE_DIR}/${INTERFACE_NAMESPACE}/${SLICE_SOURCE_BASENAME}" )
    #MESSAGE( STATUS "DEBUG: Dealing with ${SLICE_SOURCE_BASENAME}")

    #
    # Add a custom cmake command to generate each type of output file: headers and source
    #
    FOREACH ( SUFFIX ${SLICE_SUFFIXES} )

      # OUTPUT is the target we're generating rules for.
      STRING( REGEX REPLACE "\\.ice" ${SUFFIX} OUTPUT_BASENAME "${SLICE_SOURCE_BASENAME}" )
      SET( OUTPUT_FULLNAME "${SLICE2CPP_BINARY_DIR}/${INTERFACE_NAMESPACE}/${OUTPUT_BASENAME}" )

      #
      # Make each .h and .cpp file depend on _every_ slice source.  This means that if you 
      # change any .ice file everything will be recompiled. We want this because slice files
      # can include each other and if the included file has changed the downstream file needs
      # to processed again. Unfortunately CMake can't track dependencies between slice files
      # so it's safer to make them all depend on each other.
      #
      SET( DEPENDS ${GEN_SLICE_RULES_SLICE_DEPENDS} )

      #
      # For .cpp files (not .h files) add extra dependencies.
      # Let CMake know that it has to generate all the .h/.cpp files in the order listed.
      # To enforce this, make each .cpp file depend on _all_ the previously-listed .h/.cpp files.
      #
      IF( ${SUFFIX} STREQUAL ".cpp" )
        SET( DEPENDS ${DEPENDS} ${HEADERS_SO_FAR} ${BODIES_SO_FAR} )
        STRING( REGEX REPLACE "\\.cpp" ".h" ASSOCIATED_HEADER "${OUTPUT_FULLNAME}" )
        APPEND( HEADERS_SO_FAR ${ASSOCIATED_HEADER} )
        APPEND( BODIES_SO_FAR  ${OUTPUT_FULLNAME} )
      ENDIF( ${SUFFIX} STREQUAL ".cpp" )

      #
      # Add this output to the list of generated files
      #
      IF( ${SUFFIX} STREQUAL ".cpp" )
        APPEND( ${GENERATED_CPP_LIST} ${OUTPUT_BASENAME} )
        SET ( GENERATED_FILE_TYPE "source" )
      ELSE( ${SUFFIX} STREQUAL ".cpp" )
        APPEND( ${GENERATED_HEADER_LIST} ${OUTPUT_BASENAME} )
        SET ( GENERATED_FILE_TYPE "header" )
      ENDIF( ${SUFFIX} STREQUAL ".cpp" )

      #
      # Add the command to generate file.xxx from file.ice
      # Note: when the 'output' is needed, the 'command' will be called with the 'args'
      #
      #MESSAGE ( STATUS "DEBUG: Adding rule for generating ${OUTPUT_BASENAME} from ${SLICE_SOURCE_BASENAME}" )
      ADD_CUSTOM_COMMAND(
        OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_BASENAME}
        COMMAND ${SLICE2CPP_COMMAND} 
        ARGS    ${SLICE_SOURCE} ${SLICE_ARGS}
        DEPENDS ${DEPENDS}
        COMMENT "-- Generating ${GENERATED_FILE_TYPE} file from ${SLICE_SOURCE}"
        )

    ENDFOREACH ( SUFFIX ${SLICE_SUFFIXES} )

    MATH ( EXPR SLICE_SOURCE_COUNTER "${SLICE_SOURCE_COUNTER} + 1" )

  ENDFOREACH( SLICE_SOURCE_BASENAME ${ARGN} )

  # alexm:
  # as of cmake v.2.2, we don't need to explicitly set the generated files to be 'generated'.
  # see cmake FAQ entry:
  # http://www.cmake.org/Wiki/CMake_FAQ#Running_.22make_clean.22_does_not_remove_custom_command_outputs._Why.3F
  # one small problem: the last header file is not deleted on 'make clean' for some reason. don't know why?

#   MESSAGE ( STATUS "DEBUG: GENERATED_CPP_LIST: ${${GENERATED_CPP_LIST}}")
#   MESSAGE ( STATUS "DEBUG: GENERATED_HEADER_LIST: ${${GENERATED_HEADER_LIST}}")

  MESSAGE ( STATUS "Will generate cpp header and source files from ${SLICE_SOURCE_COUNTER} Slice definitions using this command:" )
  MESSAGE ( STATUS "${SLICE2CPP_COMMAND} <source.ice> ${SLICE_ARGS}" )

ENDMACRO ( GENERATE_SLICE2CPP_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )
