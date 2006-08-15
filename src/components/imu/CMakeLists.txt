OPTION( BUILD_IMU "Try to build Imu component" OFF )

IF ( BUILD_IMU )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/components/imu )
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/utils )

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

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS imucomponent imuI imudriver imuhandler fakeimudriver )

    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )
    
    # Files for the icebox service
    SET( SERVICE_SRCS imuservice ${COMMON_SRCS} )
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( imu ${COMP_SRCS} )
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/utils/serial )
#    TARGET_LINK_LIBRARIES( imu ${HONEYWELLIMU_LIBS} OrcaSerial OrcaNavUtil )
    TARGET_LINK_LIBRARIES( imu OrcaSerial OrcaNavUtil )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaImu SHARED ${SERVICE_SRCS} )
    
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
#    TARGET_LINK_LIBRARIES( OrcaImu ${HONEYWELLIMU_LIBS} OrcaSerial OrcaNavUtil )
    TARGET_LINK_LIBRARIES( OrcaImu OrcaSerial OrcaNavUtil )

    INSTALL_TARGETS( /lib OrcaImu )

    GENERATE_CONFIG_FILE( imu.def )

ENDIF ( BUILD_IMU )
