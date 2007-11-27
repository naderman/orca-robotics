IF ( NOT OS_WIN )

    # alexm: this seems like a big dependency
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaImage SHARED ${SRCS} )

    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
        TARGET_LINK_LIBRARIES( OrcaImage ${OPENCV_LIBS} )
    ENDIF ( OPENCV_FOUND )

    IF ( TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        ADD_DEFINITIONS( -DTRICLOPS_FOUND )
        TARGET_LINK_LIBRARIES( OrcaImage triclops )
    ENDIF ( TRICLOPS_FOUND )
    
    INSTALL_FILES( /include/orcaimage .*\\.h$ )

ENDIF ( NOT OS_WIN )
