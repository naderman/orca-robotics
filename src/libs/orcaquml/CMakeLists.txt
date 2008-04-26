ORCA_SET_LIBRARY_NAME( OrcaQUml )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

SET( int_libs OrcaDef )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB human_srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${human_srcs} )
    
    SET( dep_libs ${int_libs} )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} )

ENDIF( build )
