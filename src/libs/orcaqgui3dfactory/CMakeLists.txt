ORCA_SET_LIBRARY_NAME( OrcaQGui3dFactory )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} OPENSCENEGRAPH_FOUND "OpenSceneGraph not found" )
#GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GLUT_FOUND "Glut not found" )

set( dep_libs OrcaQGui OrcaQGui3d OrcaQCompModel OrcaQGuiElementUtil OrcaOgMap )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

set( dep_libs ${dep_libs} osg osgText )

if( POTRACE_FOUND )
  add_definitions( -DHAVE_POTRACE=1 )
  set( dep_libs ${dep_libs} OrcaPotrace )
endif( POTRACE_FOUND )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    file( GLOB human_srcs *.cpp )

#     set( moc_hdrs
#     )
#
#     QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${human_srcs} ${moc_srcs} )

    link_directories( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} ${QT_QTOPENGL_LIBRARY} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )
