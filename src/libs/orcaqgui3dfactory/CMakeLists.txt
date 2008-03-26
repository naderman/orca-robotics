SET( lib_name OrcaQGui3dFactory )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} OFF )
SET( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${lib_name} GLUT_FOUND "libGlut not found" )

SET( dep_libs OrcaQGui OrcaQGui3d OrcaQCompModel )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
    )

    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} ${QT_QTOPENGL_LIBRARY}
    )

    IF( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        TARGET_LINK_LIBRARIES ( ${lib_name} 3ds )
    ENDIF( LIB3DS_FOUND )

ENDIF( build )
