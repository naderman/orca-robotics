IF ( NOT OS_WIN )

OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

IF ( BUILD_FEATUREMAPLOADER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( FeatureMapLoaderSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( featuremaploader main )
    TARGET_LINK_LIBRARIES( featuremaploader FeatureMapLoaderSources )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaFeatureMapLoader SHARED service )
    TARGET_LINK_LIBRARIES( OrcaFeatureMapLoader FeatureMapLoaderSources )

    GENERATE_CONFIG_FILE( featuremaploader.def )

ENDIF ( BUILD_FEATUREMAPLOADER )
ENDIF ( NOT OS_WIN )
