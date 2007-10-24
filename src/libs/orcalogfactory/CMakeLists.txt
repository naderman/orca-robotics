IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ENDIF ( OPENCV_FOUND )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaLogFactory SHARED ${SRCS} )

    SET( COMP_DEPS OrcaLog OrcaIfaceImpl )

    # Need OpenCV to compress images to file
    IF ( OPENCV_FOUND )
        SET( COMP_DEPS ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
    ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( OrcaLogFactory ${COMP_DEPS} )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( NOT OS_WIN )
