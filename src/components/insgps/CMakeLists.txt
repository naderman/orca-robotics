SET ( COMP_NAME insgps )
SET ( SERVICE_NAME OrcaInsGps )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
REQUIRE_VAR( EXE ${COMP_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )
    
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
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroGpsUtil ${NOVATELSPANINSGPS_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
