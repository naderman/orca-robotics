#
# This file contains a set of macros used for generating 
# C++ source from SLICE files
#
# Author: Alex Brooks
#

SET ( SLICE_CPP_SUFFIXES     .cpp )
SET ( SLICE_HEADER_SUFFIXES  .h  ) 
SET ( SLICE_SUFFIXES         ${SLICE_CPP_SUFFIXES} ${SLICE_HEADER_SUFFIXES} ) 

SET ( SLICE_EXE "slice2cpp" )
SET ( SLICE_ARGS -I${PROJ_SOURCE_DIR}/src/interfaces/slice )

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
# Appends the list of .cpp files associated with the single 
# SLICE source file to the variable CPP_FILES
#
# Appends the list of header files associated with the single 
# SLICE source file to the variable HEADER_FILES
#
MACRO ( APPEND_SLICE_GENERATED_FILES SLICE_SOURCE CPP_FILES HEADER_FILES )

  SET( SUFFIXES ${SLICE_CPP_SUFFIXES} )
  FOREACH ( SUFFIX ${SUFFIXES} )
    STRING( REGEX REPLACE "\\.ice" "${SUFFIX}" CPP_FILE "${SLICE_SOURCE}" )
    APPEND( ${CPP_FILES} ${CPP_FILE} )
  ENDFOREACH ( SUFFIX ${SUFFIXES} )

  SET( SUFFIXES ${SLICE_HEADER_SUFFIXES} )
  FOREACH ( SUFFIX ${SUFFIXES} )
    STRING( REGEX REPLACE "\\.ice" "${SUFFIX}" HEADER_FILE "${SLICE_SOURCE}" )
    APPEND( ${HEADER_FILES} ${HEADER_FILE} )
  ENDFOREACH ( SUFFIX ${SUFFIXES} )

ENDMACRO ( APPEND_SLICE_GENERATED_FILES SLICE_SOURCE CPP_FILES HEADER_FILES )

#
# Generate rules for SLICE->C++ files generation, for each
# of the named .idl source files.
#
# Usage: GENERATE_SLICE( GENERATED_CPP_LIST GENERATED_HEADER_LIST [ SRC1 [ SRC2 ... ] ] )
# 
# Outputs a list of all the .cpp files that will be generated.
#
MACRO ( GENERATE_SLICE_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )

  # MESSAGE( "ARGN: ${ARGN}" )

  #
  # Loop through the SLICE sources we were given
  #
  FOREACH( SLICE_SOURCE ${ARGN} )

    #
    # Add a custom cmake command to generate each type of idl output file
    #
    FOREACH ( SUFFIX ${SLICE_SUFFIXES} )
      STRING( REGEX REPLACE "\\.ice" ${SUFFIX} OUTPUT "${SLICE_SOURCE}" )

      #
      # Get *.cpp to depend on *.h, to ensure that *.h exists before we try to compile *.cpp.
      #
      SET( FULL_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT} )

      #
      # Make each .h and .cpp file depend on _every_ slice source.  This means that if you 
      # change any .ice file everything will be recompiled.  This is done because CMake can't 
      # track dependencies between .ice files.
      #
      #SET( DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SLICE_SOURCE} )
      SET( DEPENDS ${ARGN} )
      IF( ${SUFFIX} STREQUAL ".cpp" )
        STRING( REGEX REPLACE "\\.cpp" ".h" ASSOCIATED_HEADER "${FULL_OUTPUT}" )
        SET( DEPENDS ${DEPENDS} ${ASSOCIATED_HEADER} )
      ENDIF( ${SUFFIX} STREQUAL ".cpp" )

      #
      # Add the command to generate file.xxx from file.ice
      #
      ADD_CUSTOM_TARGET(
        ${FULL_OUTPUT} ALL
        ${SLICE_EXE}
        ${CMAKE_CURRENT_SOURCE_DIR}/${SLICE_SOURCE} ${SLICE_ARGS}
        DEPENDS ${DEPENDS}
        )

      # MESSAGE( "Added custom command for ${OUTPUT}" )

    ENDFOREACH ( SUFFIX ${SLICE_SUFFIXES} )

    #
    # Work out the list of generated files
    #
    APPEND_SLICE_GENERATED_FILES ( ${SLICE_SOURCE} ${GENERATED_CPP_LIST} ${GENERATED_HEADER_LIST} )

  ENDFOREACH( SLICE_SOURCE ARGN )

  #MESSAGE( "GENERATED_CPP_LIST: ${${GENERATED_CPP_LIST}}" )

ENDMACRO ( GENERATE_SLICE_RULES GENERATED_CPP_LIST GENERATED_HEADER_LIST )


#
# Macro to do everything required to make and install a library associated
# with SLICE files
#
MACRO ( ADD_SLICE_LIB_RULES LIB_NAME SLICE_SOURCE_LIST HUMAN_SOURCE_LIST )

  #MESSAGE( "Setting up ${LIB_NAME}: SLICE_SOURCE_LIST = ${SLICE_SOURCE_LIST}" )
  #MESSAGE( "HUMAN_SOURCE_LIST = ${HUMAN_SOURCE_LIST}" )

  GENERATE_SLICE_RULES( SLICE_CPP_FILES SLICE_HEADER_FILES ${SLICE_SOURCE_LIST} )

  #MESSAGE( "SET_SOURCE_FILES_PROPERTIES(${SLICE_HEADER_FILES} PROPERTIES GENERATED true)" )

  SET_SOURCE_FILES_PROPERTIES(${SLICE_HEADER_FILES} PROPERTIES GENERATED true)
  SET_SOURCE_FILES_PROPERTIES(${SLICE_CPP_FILES} PROPERTIES GENERATED true)
  SET_DIRECTORY_PROPERTIES( 
    PROPERTIES 
    ADDITIONAL_MAKE_CLEAN_FILES 
    "${SLICE_HEADER_FILES} ${SLICE_CPP_FILES}"
    )

  ADD_LIBRARY( ${LIB_NAME} SHARED ${SLICE_CPP_FILES} ${HUMAN_SOURCE_LIST} )
  INSTALL_TARGETS( /lib ${LIB_NAME} )

  FOREACH ( FILE ${SLICE_CPP_FILES} ${SLICE_HEADER_FILES} )

    #MESSAGE( "INSTALL_FILES( /include/orca ${FILE} )" )
    INSTALL_FILES( /include/orca FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILE} )
    #INSTALL_TARGETS( /include/orca ${FILE} )

    SET_SOURCE_FILES_PROPERTIES(${FILE} PROPERTIES GENERATED true)

  ENDFOREACH ( FILE ${SLICE_CPP_FILES} ${SLICE_HEADER_FILES} )
  INSTALL_FILES( /include/orca .*\\.h$ )

ENDMACRO ( ADD_SLICE_LIB_RULES LIB_NAME SLICE_SOURCES HUMAN_SOURCES )
