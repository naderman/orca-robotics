IF ( OS_LINUX )

OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF ( BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( laserfeatureextractor ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaLaserFeatureExtractor ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( laserfeatureextractor.def )

ENDIF ( BUILD_LASERFEATUREEXTRACTOR )
ENDIF ( OS_LINUX )
