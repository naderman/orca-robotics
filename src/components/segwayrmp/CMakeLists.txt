OPTION( BUILD_SEGWAYRMP "Try to build SegwayRmp component" ON )

IF ( NOT WIN32 AND BUILD_SEGWAYRMP )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
   
    OPTIONAL_SUB_LIBRARY(
        "USB driver for Segway RMP line of platforms"
        usb
        OrcaSegwayRmpUsb
        SEGWAY_RMP_USB_LIBS
        FTD2XX_FOUND
        HAVE_USB_DRIVER
        ftd2xx usb )

#  OPTIONAL_SUB_LIBRARY( 
#    "CAN driver for Segway RMP line of platforms"
#    can
#    OrcaSegwayRmpCan
#    RM_CAN_LIBS
#    CANLIB_FOUND
#    HAVE_CAN_DRIVER
#    canlib )

# CMake problem: these should be in the subdir, but they have no effect there for some reason
INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )
    
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

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component nethandler hwhandler platform2dI position3dI powerI fakedriver )
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( segwayrmp main ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( segwayrmp ${SEGWAY_RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaSegwayRmp SHARED service ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( OrcaSegwayRmp ${SEGWAY_RMP_USB_LIBS} ${PLAYERCLIENT_LIBS} )
    
    GENERATE_CONFIG_FILE( segwayrmp.def )

ENDIF ( NOT WIN32 AND BUILD_SEGWAYRMP )
