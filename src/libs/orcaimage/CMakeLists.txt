SET ( LIB_NAME OrcaImage )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    # alexm: this seems like a big dependency
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )

    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} ${OPENCV_LIBS} )
    ENDIF ( OPENCV_FOUND )

    IF ( TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        ADD_DEFINITIONS( -DTRICLOPS_FOUND )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} triclops )
    ENDIF ( TRICLOPS_FOUND )
    
    INSTALL_FILES( /include/orcaimage .*\\.h$ )

ENDIF ( BUILD )
