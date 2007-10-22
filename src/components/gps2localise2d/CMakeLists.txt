IF ( NOT OS_WIN )

OPTION( BUILD_GPS2LOCALISE2D "Try to build Gps2Localise2d" ON )

IF ( BUILD_GPS2LOCALISE2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl OrcaGpsUtil HydroNavUtil )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( gps2localise2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( gps2localise2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaGps2Localise2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaGps2Localise2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( gps2localise2d.def )

ENDIF ( BUILD_GPS2LOCALISE2D )
ENDIF ( NOT OS_WIN )
