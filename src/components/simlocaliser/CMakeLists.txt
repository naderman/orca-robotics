SET_COMPONENT_NAMES ( SimLocaliser )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( int_libs OrcaIfaceImpl OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    # Look for Player
    INCLUDE( ${HYDRO_CMAKE_DIR}/FindPlayer2.cmake )

    # these should be in the subdir, but they have no effect there for some reason
    INCLUDE_DIRECTORIES( ${PLAYER_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${PLAYER_LINK_DIRS} )
    ADD_DEFINITIONS( ${PLAYER_DEFINITIONS} )

    OPTIONAL_SUB_LIBRARY(
        "Stage Driver for SimLocaliser"
        stage
        OrcaSimLocaliserStage
        STAGE_LIBS
        PLAYER_FOUND
        HAVE_STAGE_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} ${STAGE_LIBS} )
    
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
