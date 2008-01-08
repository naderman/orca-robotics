SET_COMPONENT_NAMES ( RegistryList )

REQUIRE_OPTION( EXE ${APP_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB COMP_SRCS *.cpp )
    
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${COMP_SRCS} )
        
    IF ( NOT OS_WIN )
        SET( COMP_DEPS IceGrid OrcaCompModel )
    ELSE ( NOT OS_WIN )
        SET( COMP_DEPS 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaCompModel )
    ENDIF ( NOT OS_WIN )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
