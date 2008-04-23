SET_COMPONENT_NAMES( ImageViewer )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not supported" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENCV_FOUND "OpenCV not found" )

SET( dep_libs OrcaImage OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    # need this for an ugly #ifdef in libOrcaImage
    ADD_DEFINITIONS( -DUSE_OPENCV )

    FILE( GLOB srcs *.cpp )
    
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
