SET_COMPONENT_NAMES ( RegistryView )
SET ( APP_CTRLC_HANDLER 0 )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build EXE ${APP_NAME} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build EXE ${APP_NAME} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaQt OrcaQCompModel )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_srcs *.cpp )

    SET( MOC_HEADERS
        mainwin.h
        regtreeview.h
        propertywidget.h
    )
    QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${HUMAN_srcs} ${MOC_srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )

    GENERATE_CONFIG_FILE( ${APP_NAME}.def )

ENDIF ( build )
