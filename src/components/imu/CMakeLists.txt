IF ( OS_LINUX  )

OPTION( BUILD_IMU "Try to build Imu component" ON )

IF ( BUILD_IMU )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/imu )
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

    # Look for Player
    INCLUDE (${ORCA_CMAKE_DIR}/FindPlayer2.cmake)

INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

#     SET( HONEYWELLIMU_FOUND 1 )
#     OPTIONAL_SUB_LIBRARY(
#         "Honeywell Driver for Imu"
#     	honewell
#         OrcaHoneywellImu
#         HONEYWELLIMU_LIBS
#         HONEYWELLIMU_FOUND
#         HAVE_HONEYWELLIMU_DRIVER
#     )

    OPTIONAL_SUB_LIBRARY(
        "Player client driver for Imu component"
        playerclientodometry3d
        OrcaOdometry3dPlayerClient
        PLAYERCLIENT_LIBS
        PLAYER_FOUND
        HAVE_PLAYERCLIENT_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroSerial HydroNavUtil OrcaIfaceImpl ${PLAYERCLIENT_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( imu ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( imu ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
    GLOBAL_ADD_LIBRARY( OrcaImu SHARED ${COMP_SRCS} service )
    TARGET_LINK_LIBRARIES( OrcaImu ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( imu.def )

ENDIF ( BUILD_IMU )
ENDIF ( OS_LINUX )
