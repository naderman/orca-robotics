SET_COMPONENT_NAMES ( OrcaView2d )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( EXE ${APP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )
    SET( COMP_DEPS OrcaQGui OrcaQGuiElementModelView OrcaQGui2d HydroDll )

#    SET( MOC_HEADERS
#        mainwin.h
#        selectableelementwidget.h
#        platformfocuscombo.h
#    )
#    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${HUMAN_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
