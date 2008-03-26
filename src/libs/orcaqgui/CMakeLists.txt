SET( lib_name OrcaQGui )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( ext_libs HydroQGui )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

SET( int_libs OrcaQt OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} )
    
    SET( moc_hdrs
        mainwin.h
        selectableelementwidget.h
        platformfocuscombo.h
    )
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} ${moc_srcs} )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )
    
    GBX_ADD_HEADERS( orcaqgui ${hdrs} )

ENDIF( build )
