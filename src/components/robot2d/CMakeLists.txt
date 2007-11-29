IF ( NOT OS_WIN )

OPTION( BUILD_ROBOT2D "Try to build Robot2d component" ON )

IF ( BUILD_ROBOT2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl OrcaObjectUtil OrcaRobotDriverUtil )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( robot2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( robot2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaRobot2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaRobot2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( robot2d.def )

ENDIF ( BUILD_ROBOT2D )
ENDIF ( NOT OS_WIN )
