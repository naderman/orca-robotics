IF ( OS_LINUX )

OPTION( BUILD_WIFI "Try to build Wifi component" ON )

IF ( BUILD_WIFI )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( WifiSources )
    
    SET( DEP_LIBS
         WifiSources
         OrcaIfaceImpl
         OrcaWifi )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( wifi main)
    TARGET_LINK_LIBRARIES( wifi ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaWifiService SHARED service )
    TARGET_LINK_LIBRARIES( OrcaWifiService ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( wifi.def )

ENDIF ( BUILD_WIFI )
ENDIF ( OS_LINUX )
