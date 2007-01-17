IF ( NOT OS_WIN )

OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( BUILD_SEGWAYRMP )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
   
    OPTIONAL_SUB_LIBRARY(
        "USB driver for Segway RMP line of platforms"
        usb
        OrcaSegwayRmpUsb
        SEGWAY_RMP_USB_LIBS
        FTD2XX_FOUND
        HAVE_USB_DRIVER
        ftd2xx usb )

    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
    
    OPTIONAL_SUB_LIBRARY(
        "Player client driver for SegwayRmp"
        playerclient
        OrcaSegwayRmpPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    IF ( NOT HAVE_USB_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )
        MESSAGE( STATUS "    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_USB_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( SegwayRmpSources )

    SET( DEP_LIBS
      SegwayRmpSources OrcaMisc OrcaIfaceImpl ${SEGWAY_RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( segwayrmp main )
    TARGET_LINK_LIBRARIES( segwayrmp ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaSegwayRmp SHARED service )
    TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${DEP_LIBS} )
    
    GENERATE_CONFIG_FILE( segwayrmp.def )

ENDIF ( BUILD_SEGWAYRMP )
ENDIF ( NOT OS_WIN )
