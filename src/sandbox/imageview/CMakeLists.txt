ORCA_SET_COMPONENT_NAME( ImageView )
# set( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( LGPL )

FIND_PACKAGE( Qt4 )
FIND_PACKAGE( OpenGL )
FIND_PACKAGE( OpenCV )

# for some reason, when the sandbox is turned on, this flag turns
# this component off again.
# set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
set( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENGL_GLU_FOUND "glu not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} OPENCV_FOUND "OpenCv not found" )

set( dep_libs HydroDll OrcaIfaceImpl )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( OPENCV_FOUND )
    include_directories( ${OPENCV_INCLUDE_DIRS} )
#    link_directories( ${OPENCV_LINK_DIRS} )
#    add_definitions( -DOPENCV_FOUND )
endif( OPENCV_FOUND )

SET ( BUILD_IMAGEVIEW FALSE )
if( BUILD_IMAGEVIEW )
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    set( QT_USE_QTOPENGL TRUE )
    include( ${QT_USE_FILE} )

    file( GLOB human_srcs *.cpp )

    set( moc_hdrs imagequeue.h viewwidget.h )
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    # build stand-alone component  
    ORCA_WRAP_MAINSUBSYSTEM_INTO_APP( ${APP_NAME} 
                                      ${APP_CTRLC_HANDLER} 
                                      ${COMP_NAMESPACE} 
                                      ${COMP_NAME} 
                                      ${human_srcs}
                                      ${moc_srcs} )
    
    target_link_libraries( ${APP_NAME} 
                           ${dep_libs} 
                           ${QT_LIBRARIES} 
                           ${OPENGL_LIBRARIES}
                           ${OPENCV_LIBS} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

endif( ORCA_BUILD_SANDBOX )
