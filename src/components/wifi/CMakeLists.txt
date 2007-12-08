OPTION( BUILD_WIFI "Try to build Wifi component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( wifi "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

IF ( BUILD AND NOT BUILD_LOCALNAV )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( wifi "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_LOCALNAV )

IF ( BUILD AND NOT HYDROWIFI_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( wifi "libHydroWifi not installed" )
ENDIF ( BUILD AND NOT HYDROWIFI_INSTALLED )


IF ( BUILD )

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

ENDIF ( BUILD )
