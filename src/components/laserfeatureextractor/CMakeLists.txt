IF ( NOT OS_WIN )

OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF ( BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( LaserFeatureExtractorSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor main )
    TARGET_LINK_LIBRARIES( laserfeatureextractor LaserFeatureExtractorSources )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED service )
    TARGET_LINK_LIBRARIES( OrcaLaserFeatureExtractor LaserFeatureExtractorSources )

    GENERATE_CONFIG_FILE( laserfeatureextractor.def )

ENDIF ( BUILD_LASERFEATUREEXTRACTOR )
ENDIF ( NOT OS_WIN )
