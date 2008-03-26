SET ( lib_name OrcaQGui3d )
GBX_ADD_LICENSE( GPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} OFF )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )
GBX_REQUIRE_VAR( build LIB ${lib_name} GLUT_FOUND "libGlut not found" )

SET( dep_libs OrcaQCompModel )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    
    SET( moc_hdrs
        worldview.h
    )
        
    QT4_WRAP_CPP( moc_srcs ${moc_hdrs} )
    
    GBX_ADD_LIBRARY( ${lib_name}  SHARED ${human_srcs} ${moc_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name}  ${dep_libs} ${QT_QTOPENGL_LIBRARY} )

    GBX_ADD_HEADERS( orcaqgui3d ${hdrs} )

ENDIF ( build )