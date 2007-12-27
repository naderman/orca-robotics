SET ( COMP_NAME orcaview2d )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${COMP_NAME} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( EXE ${COMP_NAME} HYDRODLL_INSTALLED "libHydroDll not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB HUMAN_SRCS *.cpp )

    SET( MOC_HEADERS
        mainwin.h
        selectableelementwidget.h
        platformfocuscombo.h
    )
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${HUMAN_SRCS} ${MOC_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} OrcaQGuiElementModelView OrcaQGui2d HydroDll )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
