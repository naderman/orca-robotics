OPTION( BUILD_ROBOT2D "Try to build Robot2d component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( robot2d "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_ROBOT2D )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( robot2d "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_ROBOT2D )

IF ( BUILD AND NOT HYDRODLL_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( robot2d "libHydroDll not installed" )
ENDIF ( BUILD AND NOT HYDRODLL_INSTALLED )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroDll HydroInterfaces OrcaIfaceImpl OrcaObjectUtil OrcaRobotDriverUtil )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( robot2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( robot2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaRobot2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaRobot2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )
    
    GENERATE_CONFIG_FILE( robot2d.def )

ENDIF ( BUILD )
