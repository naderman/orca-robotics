IF ( NOT OS_WIN )

OPTION( BUILD_FAITHLOCALISER "Try to build FaithLocaliser component" ON )

IF ( BUILD_FAITHLOCALISER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( FaithLocaliserSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( faithlocaliser main )
    TARGET_LINK_LIBRARIES( faithlocaliser FaithLocaliserSources )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaFaithLocaliser SHARED service )
    TARGET_LINK_LIBRARIES( OrcaFaithLocaliser FaithLocaliserSources )
    
    GENERATE_CONFIG_FILE( faithlocaliser.def )

ENDIF ( BUILD_FAITHLOCALISER )
ENDIF ( NOT OS_WIN )
