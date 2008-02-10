SET ( lib_name OrcaQGui2d )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaQCompModel OrcaQGuiElementUtil )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_srcs *.cpp detail/*.cpp )
    
    SET( MOC_HEADERS
        worldview.h
        zoomwidget.h
    )

    QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${HUMAN_srcs} ${MOC_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )
    
    INSTALL_FILES( /include/orcaqgui2d .*\\.h$ )

ENDIF ( build )
