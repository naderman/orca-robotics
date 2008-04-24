ORCA_SET_COMPONENT_NAME( RegistryList )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

SET( dep_libs OrcaCompModel )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        
    IF( NOT GBX_OS_WIN )
        SET( dep_libs ${dep_libs} OrcaCompModel )
    ELSE ( NOT GBX_OS_WIN )
        SET( dep_libs ${dep_libs} 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d )
    ENDIF( NOT GBX_OS_WIN )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
