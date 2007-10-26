OPTION( BUILD_POINT_CLOUD_SERVER "Try to build point cloud server" OFF )

IF ( BUILD_POINT_CLOUD_SERVER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pointcloudserver ${SRCS} )

#     GENERATE_CONFIG_FILE( pointcloudserver.def )

ENDIF ( BUILD_POINT_CLOUD_SERVER )
