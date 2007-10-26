IF ( NOT OS_WIN )

OPTION( BUILD_NOISYROBOT2D "Try to build NoisyRobot2d component" OFF )

IF ( BUILD_NOISYROBOT2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroNavUtil  )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( noisyrobot2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( noisyrobot2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaNoisyRobot2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaNoisyRobot2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( noisyrobot2d.def )

ENDIF ( BUILD_NOISYROBOT2D )
ENDIF ( NOT OS_WIN )
