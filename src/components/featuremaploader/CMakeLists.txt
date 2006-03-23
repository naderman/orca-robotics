OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

IF ( BUILD_FEATUREMAPLOADER )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

    INCLUDE_DIRECTORIES(
      )

    LINK_DIRECTORIES(
      )

    FILE( GLOB SRCS *.cpp )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( featuremaploader ${SRCS} )
    TARGET_LINK_LIBRARIES( featuremaploader )

    GENERATE_FROM_DEF( featuremaploader.def )

ENDIF ( BUILD_FEATUREMAPLOADER )
