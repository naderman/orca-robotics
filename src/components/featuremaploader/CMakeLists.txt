IF ( NOT OS_WIN )

OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

IF ( BUILD_FEATUREMAPLOADER )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( featuremaploader ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( featuremaploader ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaFeatureMapLoader SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaFeatureMapLoader ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( featuremaploader.def )

ENDIF ( BUILD_FEATUREMAPLOADER )
ENDIF ( NOT OS_WIN )
