OPTION( BUILD_GPS2LOCALISE2D "Try to build Gps2Localise2d" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( gps2localise2d "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_GPS2LOCALISE2D )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( gps2localise2d "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_GPS2LOCALISE2D )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl HydroGpsUtil HydroNavUtil )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( gps2localise2d ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( gps2localise2d ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaGps2Localise2d SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaGps2Localise2d ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( gps2localise2d.def )

ENDIF ( BUILD )
