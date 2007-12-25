SET ( LIB_NAME OrcaQGui2d )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp detail/*.cpp )
    
    SET( MOC_HEADERS
        worldview.h
        zoomwidget.h
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME} OrcaQCompModel OrcaQGuiElementUtil )
    
    INSTALL_FILES( /include/orcaqgui2d .*\\.h$ )

ENDIF ( BUILD )
