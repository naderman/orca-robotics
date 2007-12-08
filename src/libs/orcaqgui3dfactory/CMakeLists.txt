SET ( LIB_NAME OrcaQGui3dFactory )

REQUIRE_OPTION( LIB ${LIB_NAME} OFF )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )
REQUIRE_VAR( LIB ${LIB_NAME} GLUT_FOUND "libGlut not found" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FILE( GLOB HUMAN_SRCS *.cpp )
    
    SET( MOC_HEADERS
    )

    QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${HUMAN_SRCS} ${MOC_SRCS} )
    
    LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/qorcacm )
    TARGET_LINK_LIBRARIES ( ${LIB_NAME}
        OrcaQGui
        OrcaQGui3d
        OrcaQCompModel 
        ${QT_QTOPENGL_LIBRARY}
    )

    IF ( LIB3DS_FOUND )
        ADD_DEFINITIONS( -DHAVE_LIB3DS )
        TARGET_LINK_LIBRARIES ( ${LIB_NAME} 3ds )
    ENDIF ( LIB3DS_FOUND )

ENDIF ( BUILD )
