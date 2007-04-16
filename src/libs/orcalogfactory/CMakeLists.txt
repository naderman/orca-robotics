IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        LINK_DIRECTORIES( ${OPENCV_LINK_DIRS} )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ENDIF ( OPENCV_FOUND )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaLogFactory SHARED ${SRCS} )

    SET( DEP_LIBS OrcaNavUtil OrcaLog )

    # Need OpenCV to compress images to file
    IF ( OPENCV_FOUND )
        SET( DEP_LIBS ${DEP_LIBS} ${OPENCV_LIBS} OrcaImage )
    ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( OrcaLogFactory ${DEP_LIBS} )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( NOT OS_WIN )
