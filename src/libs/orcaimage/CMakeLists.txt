IF ( NOT WIN32 )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

    ADD_DEFINITIONS( -Wall )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaImage SHARED ${SRCS} )

    # Link libraries and include directories for digiclops/triclops and opencv
    IF ( TRICLOPS_FOUND AND OPENCV_FOUND)
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        TARGET_LINK_LIBRARIES( OrcaImage ${OPENCV_LIBS} triclops OrcaImage)
    ENDIF ( TRICLOPS_FOUND AND OPENCV_FOUND )

    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        TARGET_LINK_LIBRARIES( OrcaImage ${OPENCV_LIBS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ENDIF ( OPENCV_FOUND )

    IF ( TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        TARGET_LINK_LIBRARIES( OrcaImage triclops )
        ADD_DEFINITIONS( -DTRICLOPS_FOUND )
    ENDIF ( TRICLOPS_FOUND )
    
    
    INSTALL_FILES( /include/orcaimage .*\\.h$ )

ENDIF ( NOT WIN32 )