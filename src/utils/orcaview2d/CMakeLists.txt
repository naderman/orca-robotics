SET_COMPONENT_NAMES( OrcaView2d )
SET( APP_CTRLC_HANDLER 0 )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaQGui OrcaQGuiElementModelView OrcaObjects )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroDll )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'srcs'
    #
    FILE( GLOB human_srcs *.cpp )

#    SET( moc_hdrs
#        mainwin.h
#        selectableelementwidget.h
#        platformfocuscombo.h
#    )
#    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${human_srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF( build )
