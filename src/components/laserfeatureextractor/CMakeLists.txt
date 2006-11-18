OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF ( NOT WIN32 AND BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            component algorithmhandler
            fakedriver combineddriver svd section polefinder
            reflectorextractor foregroundextractor doorextractor cornerextractor
            polarfeature2dI laserconsumerI
    )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED service ${COMMON_SRCS} )

    GENERATE_CONFIG_FILE( laserfeatureextractor.def )

ENDIF ( NOT WIN32 AND BUILD_LASERFEATUREEXTRACTOR )
