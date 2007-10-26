OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

IF ( BUILD_LASER2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroDll HydroInterfaces OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laser2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( laser2d ${COMP_DEPS} )

    IF ( NOT OS_WIN )
        # build IceBox service
        IF ( BUILD_SERVICES )
            GLOBAL_ADD_LIBRARY( OrcaLaser2d SHARED ${COMP_SRCS} service )
            TARGET_LINK_LIBRARIES( OrcaLaser2d ${COMP_DEPS} ) 
        ENDIF ( BUILD_SERVICES )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( laser2d.def )

ENDIF ( BUILD_LASER2D )

