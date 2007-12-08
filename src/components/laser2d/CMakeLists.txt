OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND NOT BUILD_LASER2D )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( laser2d "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_LASER2D )


IF ( BUILD)

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroDll HydroInterfaces OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laser2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( laser2d ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES AND NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( OrcaLaser2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaLaser2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES AND NOT OS_WIN )

    GENERATE_CONFIG_FILE( laser2d.def )

ENDIF ( BUILD )
