ORCA_SET_COMPONENT_NAME( OrcaView3d )
set( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT_QTOPENGL_LIBRARY "QtOpenGL not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENSCENEGRAPH_FOUND "OpenSceneGraph not found" )

set( dep_libs HydroDll OrcaQCompModel OrcaQGui OrcaQGui3d )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

set( dep_libs ${dep_libs} osg osgUtil )

if( build )
    
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    file( GLOB human_srcs *.cpp )

    set( moc_hdrs
      worldview.h
    )        
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${human_srcs} ${moc_srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

    add_subdirectory( test )

endif( build )