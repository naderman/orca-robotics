SET ( lib_name OrcaQGui3d )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} OFF )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( build LIB ${lib_name} GLUT_FOUND "libGlut not found" )

SET( dep_libs OrcaQCompModel )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
        worldview.h
    )
        
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name}  SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name}  ${dep_libs} ${QT_QTOPENGL_LIBRARY} )

    INSTALL_FILES( /include/orcaqgui3d .*\\.h$ )

ENDIF ( build )