SET_COMPONENT_NAMES ( Gps )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "only Linux OS is supported" )

SET( dep_libs OrcaIfaceImpl OrcaObjects )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build )

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

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${dep_libs} ${GARMINGPS_LIBS})
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} )    
    ENDIF ( ORCA_BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
