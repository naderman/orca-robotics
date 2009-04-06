#
# This file contains a set of macros used for generating 
# Python source from Slice files
#
# Author: Ben Upcroft
# based on slice2cpp_rules.cmake
#

set( SLICE2PY_COMMAND         ${ICE_HOME}/bin/slice2py${EXE_EXTENSION} )
set( SLICE_SOURCE_DIR          ${PROJECT_SOURCE_DIR}/src/interfaces/slice )
set( SLICE_ORCA_SOURCE_DIR          ${PROJECT_SOURCE_DIR}/src/interfaces/slice/orca )
set( SLICE2PY_BINARY_DIR      ${PROJECT_BINARY_DIR}/src/interfaces/python )

# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
get_filename_component( INTERFACE_NAMESPACE ${CMAKE_CURRENT_SOURCE_DIR} NAME )

# debian package splits off slice files into a different place
if( ICE_HOME MATCHES /usr )
    set( ice_slice_dir /usr/share/slice )
    message( STATUS "This is a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
else( ICE_HOME MATCHES /usr )
    set( ice_slice_dir ${ICE_HOME}/slice )
    message( STATUS "This is NOT a Debian Ice installation. Slice files are in ${ice_slice_dir}" )
endif( ICE_HOME MATCHES /usr )

# satelite projects need to include slice files from orca installation
# the SLICE_SOURCE_DIR needs to be included in the search path as all the include files in the slice files
# are orca/<included file>. slice2py will automatically prepend orca_ to the names of each of the imported modules unless 
# the SLICE_ORCA_SOURCE_DIR directory is also included. See Sec. 22.15.2 in the Ice manual for how code generation works for
# python.  
if( DEFINED ORCA_HOME )
    set( SLICE_ARGS ${SLICE_PROJECT_ARGS} -I${SLICE_SOURCE_DIR} -I${SLICE_ORCA_SOURCE_DIR} -I${ice_slice_dir} -I${ORCA_HOME}/slice --output-dir ${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE} )
else( DEFINED ORCA_HOME )
    set( SLICE_ARGS ${SLICE_PROJECT_ARGS} -I${SLICE_SOURCE_DIR} -I${SLICE_ORCA_SOURCE_DIR} -I${ice_slice_dir} --output-dir ${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE} )
endif( DEFINED ORCA_HOME )

#
# byte-compile all the python interfaces to produce .pyc files
#
add_custom_command( 
    OUTPUT    ${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE}/orca_ice.pyc
    COMMAND   ${PYTHON_EXECUTABLE}  
    ARGS      -m compileall ${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE}
    DEPENDS   ${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE}/orca/__init__.py
    COMMENT   "Creating byte-compiled files for Python interfaces"
  )

#
# Appends the new_bit to the original.
# If the original is not set, it will be set to the new_bit.
#
macro( APPEND ORIGINAL NEW_BIT )

  IF    ( NOT ${ORIGINAL} )
    set( ${ORIGINAL} ${NEW_BIT} )
  else( NOT ${ORIGINAL} )
    set( ${ORIGINAL} ${${ORIGINAL}} ${NEW_BIT} )
  endif( NOT ${ORIGINAL} )

endmacro( APPEND ORIGINAL NEW_BIT )

#
# Generate rules for SLICE->py files generation, for each
# of the named slice source files.
#
# GENERATE_SLICE2PY_RULES( GENERATED_PY_LIST SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .py files that will be generated.
#   (ie modifies the first argument: GENERATED_PY_LIST)
#
# Sets up dependencies: 
#  - Each .py file depends on all the .ice files.
#
macro( GENERATE_SLICE2PY_RULES GENERATED_PY_LIST )

  # debug
#   message( STATUS "   slice sources    : " ${SLICE_ORCA_SOURCE_DIR} )
#   message( STATUS "   py destination  : " ${SLICE2PY_BINARY_DIR} )
#   message( STATUS "GENERATE_SLICE2PY_RULES: ARGN: ${ARGN}" )

  #
  # First pass: Loop through the SLICE sources we were given, add the full path for dependencies
  #
  foreach( SLICE_SOURCE_BASENAME ${ARGN} )
    APPEND( GEN_SLICE_RULES_SLICE_DEPENDS "${SLICE_ORCA_SOURCE_DIR}/${SLICE_SOURCE_BASENAME}" )
  endforeach( SLICE_SOURCE_BASENAME ${ARGN} )
  # message( ${GEN_SLICE_RULES_SLICE_DEPENDS} ) 

  #
  # Second pass: Loop through the SLICE sources we were given, add the CMake rules
  #
  set( SLICE_SOURCE_COUNTER 0 )
  foreach( SLICE_SOURCE_BASENAME ${ARGN} )
    set( SLICE_SOURCE "${SLICE_ORCA_SOURCE_DIR}/${SLICE_SOURCE_BASENAME}" )
    # message( STATUS "DEBUG: Dealing with ${SLICE_SOURCE_BASENAME}")

    #
    # Add a custom cmake command to generate .py output files
    #

    # OUTPUT is the target we're generating rules for.
    string( REGEX REPLACE "\\.ice" _ice.py OUTPUT_BASENAME "${SLICE_SOURCE_BASENAME}" )
    set( OUTPUT_FULLNAME "${SLICE2PY_BINARY_DIR}/${INTERFACE_NAMESPACE}/${OUTPUT_BASENAME}" )
    # message( "DEBUG: OUTPUT_BASENAME - ${OUTPUT_BASENAME}" )
    # message( "DEBUG: OUTPUT_FULLNAME - ${OUTPUT_FULLNAME}" )

    #
    # Make each .py file depend on _every_ slice source.  This means that if you 
    # change any .ice file everything will be recompiled. We want this because slice files
    # can include each other and if the included file has changed the downstream file needs
    # to processed again. Unfortunately CMake can't track dependencies between slice files
    # so it's safer to make them all depend on each other.
    #
    set( DEPENDS ${GEN_SLICE_RULES_SLICE_DEPENDS} )

    # We don't need to generate .py files in the order they are listed in
    # src/interfaces/slice/orcaslice_sources.cmake like we have to with c++ files.
    # This is because the .py files are scripts and dependencies are only checked at run time
    # once all the .py files have been generated.

    #
    # Add this output to the list of generated files
    #
    APPEND( ${GENERATED_PY_LIST} ${OUTPUT_BASENAME} )
    set( GENERATED_FILE_TYPE "python script" )

    #
    # Add the command to generate file.py from file.ice
    # Note: when the 'output' is needed, the 'command' will be called with the 'args'
    #
    # message( STATUS "DEBUG: Adding rule for generating ${OUTPUT_BASENAME} from ${SLICE_SOURCE_BASENAME}" )
    add_custom_command(
	OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_BASENAME}
	COMMAND ${SLICE2PY_COMMAND} 
	ARGS    ${SLICE_SOURCE} ${SLICE_ARGS}
	DEPENDS ${DEPENDS}
	COMMENT "-- Generating ${GENERATED_FILE_TYPE} file from ${SLICE_SOURCE}"
	)
    # message( "DEBUG: Output from custom cmake command for slice2py - ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_BASENAME}" )
    # message( "DEBUG: ARGS - ${SLICE_SOURCE} ${SLICE_ARGS}" )

    math( EXPR SLICE_SOURCE_COUNTER "${SLICE_SOURCE_COUNTER} + 1" )

  endforeach( SLICE_SOURCE_BASENAME ${ARGN} )

#   message( STATUS "DEBUG: GENERATED_PY_LIST: ${${GENERATED_PY_LIST}}")

  message( STATUS "Will generate py source files from ${SLICE_SOURCE_COUNTER} Slice definitions using this command:" )
  message( STATUS "${SLICE2PY_COMMAND} ${SLICE_ORCA_SOURCE_DIR}/*.ice ${SLICE_ARGS}" )

endmacro( GENERATE_SLICE2PY_RULES GENERATED_PY_LIST )
