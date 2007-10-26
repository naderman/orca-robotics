IF ( OS_LINUX )

OPTION( BUILD_WIFI "Try to build Wifi component" ON )

IF ( BUILD_WIFI )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl HydroWifi )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( wifi ${COMP_SRCS} main)
    TARGET_LINK_LIBRARIES( wifi ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaWifiService SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaWifiService ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( wifi.def )

ENDIF ( BUILD_WIFI )
ENDIF ( OS_LINUX )
