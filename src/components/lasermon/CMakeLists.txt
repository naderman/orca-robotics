SET_COMPONENT_NAMES ( LaserMon )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )

SET( dep_libs OrcaObjects )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
