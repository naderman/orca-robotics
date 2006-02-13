OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF( BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
            extractorone featureextractorbase laserfeatureextractorcomponent
            polarfeature2d_i fakeextractor laserconsumer_i mainloop svd section polefinder
    )
    
    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor ${COMP_SRCS} )
    
    # Files for the service
    SET( SERV_SRCS  laserfeatureextractorservice_i ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED ${SERV_SRCS} )
    
    INSTALL_TARGETS( /lib OrcaLaserFeatureExtractor )

    GENERATE_FROM_DEF( laserfeatureextractor.def )

ENDIF( BUILD_LASERFEATUREEXTRACTOR )
