OPTION( BUILD_INSGPS "Try to build InsGps component" ON )

IF ( NOT WIN32 AND BUILD_INSGPS )

    SUBDIRS( novatel )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

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
    )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS fakeinsgpsdriver component gpsI imuI position3dI handler driver)

    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )
    
    # Files for the icebox service
    SET( SERVICE_SRCS insgpsservice ${COMMON_SRCS} )
    
    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( insgps ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( insgps ${NOVATELSPANINSGPS_LIBS} OrcaSerial OrcaNavUtil )
# TARGET_LINK_LIBRARIES( insgps OrcaSerial OrcaNavUtil )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaInsGps SHARED ${SERVICE_SRCS} )
    
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaInsGps ${NOVATELSPANINSGPS_LIBS} OrcaSerial OrcaNavUtil )
#    TARGET_LINK_LIBRARIES( OrcaInsGps OrcaSerial OrcaNavUtil )

    INSTALL_TARGETS( /lib OrcaInsGps )

    GENERATE_CONFIG_FILE( insgps.def )

ENDIF ( NOT WIN32 AND BUILD_INSGPS )
