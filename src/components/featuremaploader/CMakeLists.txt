OPTION( BUILD_FEATUREMAPLOADER "Try to build FeatureMapLoader component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( featuremaploader "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_FEATUREMAPLOADER )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( featuremaploader "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_FEATUREMAPLOADER )


IF ( BUILD )

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

ENDIF ( BUILD )
