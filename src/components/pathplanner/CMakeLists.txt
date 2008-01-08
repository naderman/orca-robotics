SET_COMPONENT_NAMES ( PathPlanner )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
REQUIRE_VAR( EXE ${APP_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaOgMap  OrcaPathPlan )

    IF ( QT4_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
    ELSE( QT4_FOUND )
        LIST( REMOVE_ITEM COMP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )
        MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
    ENDIF( QT4_FOUND )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME}  ${COMP_SRCS} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
