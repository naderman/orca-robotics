ORCA_SET_COMPONENT_NAME( Def2Uml )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

SET( int_libs OrcaQUml )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF( build )

    # needs defintions from orca/ocm.ice
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} )

    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

ENDIF( build )
