OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

IF ( BUILD_LASER2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/laser2d )

    # Driver implementations
    ADD_SUBDIRECTORY( fakedriver )
    ADD_SUBDIRECTORY( sickacfr )
    ADD_SUBDIRECTORY( sickcarmen )
    ADD_SUBDIRECTORY( playerclient )
    
    #
    # General stuff for all drivers
    #
    ADD_SUBDIRECTORY ( laser2dutil )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaLaser2dUtil HydroDll OrcaIfaceImpl )

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

