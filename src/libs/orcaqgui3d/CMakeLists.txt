ORCA_SET_LIBRARY_NAME( OrcaQGui3d )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} OPENSCENEGRAPH_FOUND "OpenSceneGraph not found" )

set( dep_libs OrcaQCompModel )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    set( dep_libs ${dep_libs} osg osgUtil )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    link_directories( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} ${QT_QTOPENGL_LIBRARY} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
      add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )