ORCA_SET_COMPONENT_NAME( ImageViewQt )
set( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( GPL )

FIND_PACKAGE( Qt4 )
FIND_PACKAGE( OpenGL )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENGL_GLU_FOUND "glu not found" )

set( dep_libs HydroDll HydroGlu OrcaIfaceImpl OrcaObj )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    set( QT_USE_QTOPENGL TRUE )
    include( ${QT_USE_FILE} )

    file( GLOB human_srcs *.cpp )

    set( moc_hdrs imagequeue.h viewwidget.h )
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${human_srcs} ${moc_srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( build )
