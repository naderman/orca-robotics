ORCA_SET_LIBRARY_NAME( OrcaQGui3dFactory )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} OPENSCENEGRAPH_FOUND "OpenSceneGraph not found" )

SET( dep_libs OrcaQGui OrcaQGui3d OrcaQCompModel OrcaQGuiElementUtil OrcaOgMap )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${dep_libs} )

SET( ext_libs osg osgText )

IF ( POTRACE_FOUND )
  ADD_DEFINITIONS( -DHAVE_POTRACE=1 )
  SET( ext_libs ${ext_libs} OrcaPotrace )
ENDIF ( POTRACE_FOUND )

IF( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} ${dep_libs} ${ext_libs} ${QT_QTOPENGL_LIBRARY} )

    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
