ORCA_SET_LIBRARY_NAME( HydroQGuiPath )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

if( build )

    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )
    include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    file( GLOB hdrs *.h )
    file( GLOB human_srcs *.cpp )

    set( moc_hdrs
        pathdesigntablewidget.h
        pathfollowerbuttons.h
        pathplannerbuttons.h
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${human_srcs} ${moc_srcs} )

    GBX_ADD_HEADERS( hydroqguipath ${hdrs} )

endif( build )
