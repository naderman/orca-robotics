OPTION( BUILD_GPS "Try to build Gps component" ON )

IF ( NOT WIN32 AND NOT QNXNTO )
IF ( BUILD_GPS )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    INCLUDE_DIRECTORIES( 
      ${PROJECT_BINARY_DIR}
      ${PROJECT_SOURCE_DIR}/src/components/gps
    )

    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
    SET( ASHTECHGPS_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Ashtech Driver for Gps"
    	ashtech
        OrcaAshtechGps
        ASHTECHGPS_LIBS
        ASHTECHGPS_FOUND
        HAVE_ASHTECHGPS_DRIVER
    )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS gpscomponent gpsI gpsdriver gpshandler fakegpsdriver
            nmea )

    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )
    
    # Files for the icebox service
    SET( SERVICE_SRCS gpsservice ${COMMON_SRCS} )
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( gps ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( gps ${ASHTECHGPS_LIBS} OrcaSerial OrcaNavUtil )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaGps SHARED ${SERVICE_SRCS} )
    
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaGps ${ASHTECHGPS_LIBS} OrcaSerial OrcaNavUtil )

    INSTALL_TARGETS( /lib OrcaGps )

    GENERATE_CONFIG_FILE( gps.def )

ENDIF ( BUILD_GPS )
ENDIF ( NOT WIN32 AND NOT QNXNTO )

