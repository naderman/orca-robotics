OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

IF ( NOT WIN32 AND BUILD_FEATUREMAPLOADER )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component featuremap2dI fakemaploader )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( featuremaploader main ${COMMON_SRCS} )

    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaFeatureMapLoader SHARED service ${COMMON_SRCS} )

    GENERATE_CONFIG_FILE( featuremaploader.def )

ENDIF ( NOT WIN32 AND BUILD_FEATUREMAPLOADER )
