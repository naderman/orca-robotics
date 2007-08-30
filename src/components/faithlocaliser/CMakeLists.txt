IF ( NOT OS_WIN )

OPTION( BUILD_FAITHLOCALISER "Try to build FaithLocaliser component" ON )

IF ( BUILD_FAITHLOCALISER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( faithlocaliser ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( faithlocaliser ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaFaithLocaliser SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaFaithLocaliser ${COMP_DEPS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( faithlocaliser.def )

ENDIF ( BUILD_FAITHLOCALISER )
ENDIF ( NOT OS_WIN )
