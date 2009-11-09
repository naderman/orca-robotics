ORCA_SET_LIBRARY_NAME( OrcaQGuiElementModelView )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

set( dep_libs HydroQGui HydroQGuiElementUtil OrcaQt OrcaQCompModel )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    file( GLOB hdrs *.h )
    file( GLOB human_srcs *.cpp )

    set( moc_hdrs
        guielementmodel.h
        guielementview.h
        regselectview.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${human_srcs} ${moc_srcs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

endif( build )
