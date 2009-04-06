set( build TRUE )

if( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.4 AND ${CMAKE_PATCH_VERSION} LESS 6 )
    set( build FALSE )
    message( "-- Python interfaces - CANNOT be built.")
    message( "-- Python interfaces requires at least CMake v2.4.6. You are running v${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}. Please upgrade." )
endif( ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} EQUAL 2.4 AND ${CMAKE_PATCH_VERSION} LESS 6 )

if( build )

    if( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )
    
    message( STATUS "Planning to build Python interfaces." )
    
    #
    # Install sub-directory will be the same as the current sub-directory
    # which is typically the same as the name of the namespace, e.g. 'orca'
    #
    get_filename_component( slice_module ${CMAKE_CURRENT_SOURCE_DIR} NAME )
    
    #
    # All Slice source files are defined in one place.
    #
    include( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${slice_module}/slice_sources.cmake )
    
    #
    # Defines macros for compiling Slice source files.
    #
    include( ${ORCA_CMAKE_DIR}/slice2python_rules.cmake )
    
    #
    # Work out the list of generated files from the list of slice sources
    #
    GENERATE_SLICE2PY_RULES( SLICE_GENERATED_PY_FILES ${ORCA_SLICE_SOURCE_FILES} )
    
    #
    # tell "make" to create the .py targets by looping through over each <interface>.py
    #
    foreach( PYTHON_INTERFACE ${SLICE_GENERATED_PY_FILES} )
	# based off the wiki entry on compliling latex files with cmake:
	# http://www.cmake.org/Wiki/CMake_FAQ#How_do_I_use_CMake_to_build_LaTeX_documents.3F 
	ADD_CUSTOM_TARGET( ${PYTHON_INTERFACE} ALL echo
	    DEPENDS ${SLICE2PY_BINARY_DIR}/${slice_module}/${ORCA_SLICE_SOURCE_FILES} )
    endforeach( PYTHON_INTERFACE ${SLICE_GENERATED_PY_FILES} )
    
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
    string( REGEX REPLACE "\\.py" .pyc SLICE_GENERATED_PYC_FILES "${SLICE_GENERATED_PY_FILES}" )
    INSTALL_FILES( /python/${slice_module} FILES ${SLICE_GENERATED_PYC_FILES} )
    # install the orca python module
    install(DIRECTORY ${SLICE2PY_BINARY_DIR}/${slice_module} DESTINATION ${GBX_SHARE_INSTALL_DIR}/python/${slice_module} )
    message( STATUS "Will install Python files into ${GBX_SHARE_INSTALL_DIR}/python/${slice_module}" )
    
    else( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )
	message( STATUS "Will not build Python interfaces : PYTHONINTERP_FOUND=${PYTHON_FOUND}, ORCA_BUILD_PYTHON=${ORCA_BUILD_PYTHON}" )
    endif( PYTHONINTERP_FOUND AND ORCA_BUILD_PYTHON )

endif( build )
