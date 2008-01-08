SET_COMPONENT_NAMES ( RegistryView )

REQUIRE_OPTION( EXE ${APP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${APP_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )

    SET( MOC_HEADERS
        mainwin.h
        regtreeview.h
        propertywidget.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

    GLOBAL_ADD_EXECUTABLE( ${APP_NAME} ${HUMAN_SRCS} ${MOC_SRCS} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} OrcaQt OrcaQCompModel )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( BUILD )
