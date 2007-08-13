IF ( OS_LINUX )

OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF ( BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( LaserFeatureExtractorSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor main )
    TARGET_LINK_LIBRARIES( laserfeatureextractor LaserFeatureExtractorSources OrcaIfaceImpl )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED service )
    TARGET_LINK_LIBRARIES( OrcaLaserFeatureExtractor LaserFeatureExtractorSources OrcaIfaceImpl )

    GENERATE_CONFIG_FILE( laserfeatureextractor.def )

ENDIF ( BUILD_LASERFEATUREEXTRACTOR )
ENDIF ( OS_LINUX )
