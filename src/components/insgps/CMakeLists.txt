IF ( OS_LINUX )

OPTION( BUILD_INSGPS "Try to build InsGps component" OFF )

IF ( BUILD_INSGPS )

    ADD_SUBDIRECTORY( novatel )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    SET( INSGPS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
    INCLUDE_DIRECTORIES( ${INSGPS_SRC_DIR} )

    # OPTIONAL_SUB_LIBRARY macro defined in component_rules.cmake
    SET( NOVATELSPANINSGPS_FOUND 1 )
    OPTIONAL_SUB_LIBRARY(
        "Novatel Span Driver for InsGps"
    	novatelspan
        OrcaNovatelSpanInsGps
        NOVATELSPANINSGPS_LIBS
        NOVATELSPANINSGPS_FOUND
        HAVE_NOVATELSPANINSGPS_DRIVER
        HydroSerial OrcaNovatel
    )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaGpsUtil ${NOVATELSPANINSGPS_LIBS} )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( insgps ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( insgps ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaInsGps SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaInsGps ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( insgps.def )

ENDIF ( BUILD_INSGPS )
ENDIF ( OS_LINUX )
