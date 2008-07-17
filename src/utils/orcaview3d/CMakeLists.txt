ORCA_SET_COMPONENT_NAME( OrcaView3d )
SET( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} OFF )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} not_os_win "Windows OS is not  supported" )

SET( dep_libs OrcaQCompModel OrcaQGui OrcaQGui3d )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

GBX_REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GLUT_FOUND "libGlut not found" )
SET( dep_libs ${dep_libs} HydroDll glut )

IF( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'srcs'
    #
    FILE( GLOB human_srcs component.cpp worldview.cpp viewhandler.cpp camera.cpp vector3.cpp matrix3.cpp matrix4.cpp )

    SET( moc_hdrs
      worldview.h
    )        
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )

    # build stand-alone component  
    ORCA_WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${human_srcs} ${moc_srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

    IF( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        TARGET_LINK_LIBRARIES ( ${LIB_NAME} 3ds )
    ENDIF( LIB3DS_FOUND )

    GBX_ADD_EXECUTABLE( simple viewhandler.cpp camera.cpp vector3.cpp matrix3.cpp matrix4.cpp simple.cpp )
    TARGET_LINK_LIBRARIES( simple ${dep_libs} )

ENDIF( build )