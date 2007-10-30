
IF ( OS_LINUX )

OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( BUILD_SEGWAYRMP )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # Look for Player
    INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )    

    ADD_SUBDIRECTORY( rmpdriver )

    IF ( FTD2XX_FOUND )
      MESSAGE( STATUS "    USB driver for Segway RMP line of platforms -- can be built" )
      SET( SEGWAY_RMP_USB_LIBS OrcaSegwayRmpUsb )
      LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/rmpdriver/usb )
      ADD_DEFINITIONS( -DHAVE_USB_DRIVER )
      SET( HAVE_USB_DRIVER TRUE )
    ELSE ( FTD2XX_FOUND )
      MESSAGE( STATUS "    USB driver for Segway RMP line of platforms -- CANNOT be built" )
    ENDIF ( FTD2XX_FOUND )

    OPTION( BUILD_PEAKCAN "Try to build the optional peak CAN drivers" ON )
    
    IF ( PEAKCAN_FOUND AND BUILD_PEAKCAN )
      MESSAGE( STATUS "    Peak CAN driver for Segway RMP line of platforms -- can be built" )
      SET( SEGWAY_RMP_CAN_LIBS OrcaSegwayRmpCan )
      LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/rmpdriver/can )
      ADD_DEFINITIONS( -DHAVE_CAN_DRIVER )
      SET( HAVE_CAN_DRIVER TRUE )
    ELSE ( PEAKCAN_FOUND AND BUILD_PEAKCAN )
      MESSAGE( STATUS "    Peak CAN driver for Segway RMP line of platforms -- CANNOT be built" )
	  SET ( BUILD_PEAKCAN FALSE ) 
    ENDIF ( PEAKCAN_FOUND AND BUILD_PEAKCAN )

    OPTIONAL_SUB_LIBRARY(
        "Player client driver for SegwayRmp"
        playerclient
        OrcaSegwayRmpPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    IF ( NOT HAVE_USB_DRIVER AND NOT HAVE_CAN_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )
        MESSAGE( STATUS "    WARNING: only default 'fake' driver can be built")
    ENDIF ( NOT HAVE_USB_DRIVER AND NOT HAVE_CAN_DRIVER AND NOT HAVE_PLAYERCLIENT_DRIVER )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS   OrcaIfaceImpl OrcaSegwayRmpDriver OrcaObjectUtil OrcaRobotDriverUtil
                    ${SEGWAY_RMP_USB_LIBS} ${SEGWAY_RMP_CAN_LIBS} ${PLAYERCLIENT_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( segwayrmp ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( segwayrmp ${COMP_DEPS} )

    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaSegwayRmp SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( segwayrmp.def )

ENDIF ( BUILD_SEGWAYRMP )
ENDIF ( OS_LINUX )
