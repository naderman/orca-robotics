SET_COMPONENT_NAMES ( RegistryList )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )

SET( dep_libs OrcaCompModel )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        
    IF ( NOT ORCA_OS_WIN )
        SET( dep_libs ${dep_libs} OrcaCompModel )
    ELSE ( NOT ORCA_OS_WIN )
        SET( dep_libs ${dep_libs} 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d )
    ENDIF ( NOT ORCA_OS_WIN )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
