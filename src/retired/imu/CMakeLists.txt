#
# THIS COMPONENT IS HAS NOT BEEN UPDATED IN A WHILE
# DO NOT USE IT A TEMPLATE FOR NEW DEVELOPMENT
#

IF ( ORCA_OS_LINUX  )

OPTION( build_IMU "Try to build Imu component" OFF )

IF ( build_IMU )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/imu )
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR} )

    # Look for Player
    INCLUDE (${HYDRO_HOME}/cmake/FindPlayer2.cmake)

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

    FIND_COMPONENT_SOURCES( srcs )
    SET( dep_libs HydroSerial HydroNavUtil OrcaIfaceImpl ${PLAYERCLIENT_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( imu ${srcs} main )
    TARGET_LINK_LIBRARIES( imu ${COMP_DEPS} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
    GLOBAL_ADD_LIBRARY( OrcaImu SHARED ${srcs} service )
    TARGET_LINK_LIBRARIES( OrcaImu ${COMP_DEPS} ) 
    ENDIF ( ORCA_BUILD_SERVICES )

    GENERATE_CONFIG_FILE( imu.def )

ENDIF ( build_IMU )
ENDIF ( ORCA_OS_LINUX )
