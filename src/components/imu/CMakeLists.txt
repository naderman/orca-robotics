IF ( NOT OS_WIN  )

OPTION( BUILD_IMU "Try to build Imu component" ON )

IF ( BUILD_IMU )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/imu )
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )


#     # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
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

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( OrcaImuSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( imu main )
    TARGET_LINK_LIBRARIES( imu OrcaImuSources OrcaSerial OrcaNavUtil OrcaIfaceImpl ${PLAYERCLIENT_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaImu SHARED service )
    TARGET_LINK_LIBRARIES( OrcaImu OrcaImuSources OrcaSerial OrcaNavUtil OrcaIfaceImpl ${PLAYERCLIENT_LIBS} )

    GENERATE_CONFIG_FILE( imu.def )

ENDIF ( BUILD_IMU )
ENDIF ( NOT OS_WIN )
