ORCA_SET_LIBRARY_NAME( OrcaQGui )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

SET( ext_libs HydroQGui )
GBX_REQUIRE_INSTALLS( build LIB ${LIB_NAME} ${ext_libs} )

SET( int_libs OrcaQt OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} )
    
    SET( moc_hdrs
        mainwin.h
        selectableelementwidget.h
        platformfocuscombo.h
    )
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${human_srcs} ${moc_srcs} )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
