SET ( LIB_NAME OrcaQGui )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
        mainwin.h
        selectableelementwidget.h
        platformfocuscombo.h
    )
    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} OrcaQt )
    
    INSTALL_FILES( /include/orcaqgui .*\\.h$ )

ENDIF ( BUILD )
