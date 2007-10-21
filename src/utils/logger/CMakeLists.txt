IF( NOT OS_WIN )

OPTION( BUILD_LOGGER "Try to build Logger" ON )

IF( BUILD_LOGGER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaLog HydroDll )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( logger ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( logger ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaLogger SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaLogger ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( logger.def )

ENDIF( BUILD_LOGGER )
ENDIF( NOT OS_WIN )
