ORCA_SET_LIBRARY_NAME( OrcaQGuiElementModelView )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

SET( ext_libs HydroQGui HydroQGuiElementUtil)
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} HYDROQGUI_INSTALLED "libHydroQGui not installed" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} HYDROQGUIELEMENTUTIL_INSTALLED "HydroQGuiElementUtil not installed" )

SET( int_libs OrcaQt OrcaQCompModel )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )


IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
        guielementmodel.h
        guielementview.h
        regselectview.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${human_srcs} ${moc_srcs} )
    
    SET( dep_libs ${ext_libs} ${int_libs} )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
