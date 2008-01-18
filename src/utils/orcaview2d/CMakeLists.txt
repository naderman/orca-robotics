SET_COMPONENT_NAMES ( OrcaView2d )
SET ( APP_CTRLC_HANDLER 0 )
GLOBAL_ADD_LICENSE( GPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

SET( DEP_LIBS OrcaQGui OrcaQGuiElementModelView OrcaQGui2d )
REQUIRE_TARGETS( BUILD EXE ${APP_NAME} ${DEP_LIBS} )

SET( DEP_LIBS ${DEP_LIBS} HydroDll )
REQUIRE_VAR( BUILD EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )
REQUIRE_VAR( BUILD EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )

#    SET( MOC_HEADERS
#        mainwin.h
#        selectableelementwidget.h
#        platformfocuscombo.h
#    )
#    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${HUMAN_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
