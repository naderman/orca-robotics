ORCA_SET_COMPONENT_NAME( SimLocaliser )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

set( dep_libs OrcaIfaceImpl OrcaIfaceUtil OrcaObjects )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    # Obsolete but still used here
    include( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

    # Look for Player
    include( ${ORCA_CMAKE_DIR}/FindPlayer2.cmake )

    # these should be in the subdir, but they have no effect there for some reason
    include_directories( ${PLAYER_INCLUDE_DIRS} )
    link_directories( ${PLAYER_LINK_DIRS} )
    add_definitions( ${PLAYER_DEFINITIONS} )

    ORCA_OPTIONAL_SUB_LIBRARY(
        "Stage Driver for SimLocaliser"
        stage
        OrcaSimLocaliserStage
        STAGE_LIBS
        PLAYER_FOUND
        HAVE_STAGE_DRIVER
        ${PLAYER_LIBS} OrcaPlayer
    )

    file( GLOB srcs *.cpp )
    set( dep_libs ${dep_libs} ${STAGE_LIBS} )
    
    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    if( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${srcs} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} )    
    endif( ORCA_BUILD_SERVICES )
    
    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
