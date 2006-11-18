OPTION( BUILD_POINT_CLOUD_SERVER "Try to build point cloud server" ON )

IF ( BUILD_POINT_CLOUD_SERVER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    

    # this driver can always be built
    MESSAGE( STATUS "Point cloud server can be built" )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pointcloudserver main pcserver )
    
    # build an IceBox service
    # GLOBAL_ADD_LIBRARY( OrcaReactiveWalker SHARED service ${COMMON_SRCS} )

    # GENERATE_CONFIG_FILE( pointcloudserver.def )

ENDIF ( BUILD_POINT_CLOUD_SERVER )
