OPTION( BUILD_INSGPS "Try to build InsGps component" ON )
SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( insgps "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

IF ( BUILD AND NOT BUILD_INSGPS )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( insgps "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_INSGPS )


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
    GLOBAL_ADD_EXECUTABLE( insgps ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( insgps ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaInsGps SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaInsGps ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( insgps.def )

ENDIF ( BUILD )
