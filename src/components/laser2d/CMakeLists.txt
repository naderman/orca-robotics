OPTION( BUILD_LASER2D "Try to build Laser2d component" ON )

IF ( BUILD_LASER2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/laser2d )

    # Driver implementations
    ADD_SUBDIRECTORY( fakedriver )
    ADD_SUBDIRECTORY( sickcarmen )
    ADD_SUBDIRECTORY( sickacfr )
    ADD_SUBDIRECTORY( playerclient )
    
    #
    # General stuff for all drivers
    #
    ADD_SUBDIRECTORY ( laser2dutil )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( Laser2dSources )

    SET( DEP_LIBS
      Laser2dSources
      OrcaLaser2dUtil
      OrcaDynamicLoad
      OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laser2d main )
    TARGET_LINK_LIBRARIES( laser2d ${DEP_LIBS} )

    IF ( NOT OS_WIN )
        # build IceBox service
        GLOBAL_ADD_LIBRARY( OrcaLaser2d SHARED service )
        TARGET_LINK_LIBRARIES( OrcaLaser2d ${DEP_LIBS} )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( laser2d.def )

ENDIF ( BUILD_LASER2D )

