OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

IF ( BUILD_FEATUREMAPLOADER )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component component featuremap2dI fakemaploader )
    
    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )

    # Files for the icebox service
    SET( SERVICE_SRCS serviceI ${COMMON_SRCS} )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( featuremaploader ${COMP_SRCS} )

    # build an IceBox service
    ADD_LIBRARY( OrcaFeatureMapLoader SHARED ${SERVICE_SRCS} )
    INSTALL_TARGETS( /lib OrcaFeatureMapLoader )

    GENERATE_FROM_DEF( featuremaploader.def )

ENDIF ( BUILD_FEATUREMAPLOADER )
