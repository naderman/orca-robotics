SET_COMPONENT_NAMES ( RegistryList )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )

SET( DEP_LIBS OrcaCompModel )
REQUIRE_TARGETS( BUILD EXE ${APP_NAME} ${DEP_LIBS} )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
        
    IF ( NOT OS_WIN )
        SET( DEP_LIBS ${DEP_LIBS} OrcaCompModel )
    ELSE ( NOT OS_WIN )
        SET( DEP_LIBS ${DEP_LIBS} 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d )
    ENDIF ( NOT OS_WIN )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
