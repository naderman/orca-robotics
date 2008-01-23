SET_COMPONENT_NAMES ( Gps )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )
REQUIRE_VAR( BUILD EXE ${APP_NAME} OS_LINUX "only Linux OS is supported" )

SET( DEP_LIBS OrcaIfaceImpl OrcaObjects )
REQUIRE_TARGETS( BUILD EXE ${APP_NAME} ${DEP_LIBS} )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
      ${PROJECT_BINARY_DIR}
      ${PROJECT_SOURCE_DIR}/src/components/${COMP_NAME}
    )

    SET( GARMINGPS_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Garmin Driver for Gps"
    	garmin
        OrcaGarminGps
        GARMINGPS_LIBS
        GARMINGPS_FOUND
        HAVE_GARMINGPS_DRIVER
        HydroSerial
        HydroGpsUtil
    )

    FILE( GLOB COMP_SRCS *.cpp )
    SET( DEP_LIBS ${DEP_LIBS} ${GARMINGPS_LIBS})
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${DEP_LIBS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
