SET_COMPONENT_NAMES( PathPlanner )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroPathPlan )
GBX_REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( int_libs OrcaOgMap OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} )

    IF( QT4_FOUND )
        INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
        ADD_DEFINITIONS( -DQT4_FOUND )
    ELSE( QT4_FOUND )
        LIST( REMOVE_ITEM srcs "${CMAKE_CURRENT_SOURCE_DIR}/skeletongraphicsI.cpp" )
        MESSAGE( STATUS "     Qt4 not found: skeletongraphics will not be built" )
    ENDIF( QT4_FOUND )
    
    # build stand-alone component  
    WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF( ORCA_BUILD_SERVICES )
        WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} )    
    ENDIF( ORCA_BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
