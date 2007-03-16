IF ( OS_LINUX OR OS_QNX )

OPTION( BUILD_INSGPS "Try to build InsGps component" ON )

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
        OrcaSerial novatel
    )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( InsGpsSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( insgps main )
    TARGET_LINK_LIBRARIES( insgps InsGpsSources ${NOVATELSPANINSGPS_LIBS} OrcaGpsUtil )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaInsGps SHARED service )
    TARGET_LINK_LIBRARIES( OrcaInsGps InsGpsSources ${NOVATELSPANINSGPS_LIBS} OrcaGpsUtil )

    GENERATE_CONFIG_FILE( insgps.def )

ENDIF ( BUILD_INSGPS )
ENDIF ( OS_LINUX OR OS_QNX )
