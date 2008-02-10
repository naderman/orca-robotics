SET ( lib_name OrcaQGui3dFactory )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} OFF )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( build LIB ${lib_name} GLUT_FOUND "libGlut not found" )

SET( dep_libs OrcaQGui OrcaQGui3d OrcaQCompModel )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_srcs *.cpp )
    
    SET( MOC_HEADERS
    )

    QT4_WRAP_CPP( MOC_srcs ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${HUMAN_srcs} ${MOC_srcs} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} ${QT_QTOPENGL_LIBRARY}
    )

    IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        TARGET_LINK_LIBRARIES ( ${lib_name} 3ds )
    ENDIF ( LIB3DS_FOUND )

ENDIF ( build )
