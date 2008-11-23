SET( build TRUE )

IF( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.4 AND ${CMAKE_PATCH_VERSION} LESS 6 )
    SET( build FALSE )
    MESSAGE( "-- Python interfaces - CANNOT be built.")
    MESSAGE( "-- Python interfaces requires at least CMake v2.4.6. You are running v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}. Please upgrade." )
ENDIF( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.4 AND ${CMAKE_PATCH_VERSION} LESS 6 )

IF( build )

    IF( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )
    
    MESSAGE( STATUS "Planning to build Python interfaces." )
    
    #
    # Install sub-directory will be the same as the current sub-directory
    # which is typically the same as the name of the namespace, e.g. 'orca'
    #
    GET_FILENAME_COMPONENT( slice_module ${CMAKE_CURRENT_SOURCE_DIR} NAME )
    
    #
    # All Slice source files are defined in one place.
    #
    INCLUDE( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${slice_module}/slice_sources.cmake )
    
    #
    # Defines macros for compiling Slice source files.
    #
    INCLUDE( ${ORCA_CMAKE_DIR}/slice2python_rules.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    GENERATE_SLICE2PY_RULES( SLICE_GENERATED_PY_FILES ${ORCA_SLICE_SOURCE_FILES} )
    
    #
    # tell "make" to create the .py targets by looping through over each <interface>.py
    #
    FOREACH( PYTHON_INTERFACE ${SLICE_GENERATED_PY_FILES} )
	# based off the wiki entry on compliling latex files with cmake:
	# http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_CMake_to_build_LaTeX_documents.3F 
	ADD_CUSTOM_TARGET( ${PYTHON_INTERFACE} ALL echo
	    DEPENDS ${SLICE2PY_BINARY_DIR}/${slice_module}/${ORCA_SLICE_SOURCE_FILES} )
    ENDFOREACH( PYTHON_INTERFACE ${SLICE_GENERATED_PY_FILES} )
    
    #
    # Tell "make" to crate the .pyc byte compiled python files.
    # See slice2python_rules.cmake for the custom command for compiling all the .py files
    #
    ADD_CUSTOM_TARGET(orca_ice.pyc ALL echo
	DEPENDS   ${SLICE2PY_BINARY_DIR}/${slice_module}/orca_ice.pyc
	)
    
    #
    # install the .py, .pyc files, and the package for the orca python module
    #
    
    # install .py files
    INSTALL_FILES( /python/${slice_module} FILES ${SLICE_GENERATED_PY_FILES} )
    # install .pyc files
    STRING( REGEX REPLACE "\\.py" .pyc SLICE_GENERATED_PYC_FILES "${SLICE_GENERATED_PY_FILES}" )
    INSTALL_FILES( /python/${slice_module} FILES ${SLICE_GENERATED_PYC_FILES} )
    # install the orca python module
    INSTALL(DIRECTORY ${SLICE2PY_BINARY_DIR}/${slice_module} DESTINATION ${GBX_SHARE_INSTALL_DIR}/python/${slice_module} )
    MESSAGE( STATUS "Will install Python files into ${GBX_SHARE_INSTALL_DIR}/python/${slice_module}" )
    
    ELSE ( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )
	MESSAGE( STATUS "Will not build Python interfaces : PYTHONINTERP_FOUND=${PYTHON_FOUND}, ORCA_BUILD_PYTHON=${ORCA_BUILD_PYTHON}" )
    ENDIF( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )

ENDIF( build )