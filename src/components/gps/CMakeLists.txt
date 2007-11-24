IF ( OS_LINUX )

OPTION( BUILD_GPS "Try to build Gps component" ON )

IF ( BUILD_GPS )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
      ${PROJECT_BINARY_DIR}
      ${PROJECT_SOURCE_DIR}/src/components/gps
    )

    SET( GARMINGPS_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Garmin Driver for Gps"
    	garmin
        OrcaGarminGps
        GARMINGPS_LIBS
        GARMINGPS_FOUND
        HAVE_GARMINGPS_DRIVER
        HydroSerial
        HydroGpsUtil
    )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl ${ASHTECHGPS_LIBS} ${GARMINGPS_LIBS})
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( gps ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( gps ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaGps SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaGps ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( gps.def )

ENDIF ( BUILD_GPS )
ENDIF ( OS_LINUX )

