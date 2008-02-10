SET_COMPONENT_NAMES ( InsGps )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "only Linux OS is supported" )

SET( dep_libs ${dep_libs} HydroGpsUtil )
REQUIRE_VAR( build EXE ${APP_NAME} HYDROGPSUTIL_INSTALLED "libHydroGpsUtil not installed" )

IF ( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    SET( INSGPS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
    INCLUDE_DIRECTORIES( ${INSGPS_SRC_DIR} )

    SET( NOVATELSPANINSGPS_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Novatel Span Driver for InsGps"
    	novatelspan
        OrcaNovatelSpanInsGps
        NOVATELSPANINSGPS_LIBS
        NOVATELSPANINSGPS_FOUND
        HAVE_NOVATELSPANINSGPS_DRIVER
        HydroSerial HydroNovatelUtil
    )
    
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${dep_libs} ${NOVATELSPANINSGPS_LIBS} )
    
    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} ) 
    ENDIF ( ORCA_BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
