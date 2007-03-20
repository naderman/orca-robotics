IF ( NOT OS_WIN )

OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( BUILD_SEGWAYRMP )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

    ADD_SUBDIRECTORY( rmpdriver )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( SegwayRmpSources )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( segwayrmp main )

    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaSegwayRmp SHARED service )

    IF ( FTD2XX_FOUND )
      MESSAGE( "USB driver for Segway RMP line of platforms -- can be built" )
      TARGET_LINK_LIBRARIES( segwayrmp OrcaSegwayRmpUsb )
      ADD_DEFINITIONS( -DHAVE_USB_DRIVER )
      SET( HAVE_USB_DRIVER TRUE )
    ELSE ( FTD2XX_FOUND )
      MESSAGE( "USB driver for Segway RMP line of platforms -- cannot be built" )
    ENDIF ( FTD2XX_FOUND )

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
    
    SET( DEP_LIBS SegwayRmpSources 
      OrcaSegwayRmpDriver
      OrcaMisc 
      OrcaIfaceImpl 
      ${SEGWAY_RMP_USB_LIBS} 
      ${PLAYERCLIENT_LIBS} )
    
    TARGET_LINK_LIBRARIES( segwayrmp ${DEP_LIBS} )
    TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( segwayrmp.def )

ENDIF ( BUILD_SEGWAYRMP )
ENDIF ( NOT OS_WIN )
