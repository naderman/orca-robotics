IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaLogFactory SHARED ${SRCS} )
    
    # Need OpenCV to compress images to file
    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${OPENCV_INCLUDE_DIRS} )
        TARGET_LINK_LIBRARIES( OrcaLogFactory OrcaNavUtil ${OPENCV_LIBS} OrcaImage )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ELSE ( OPENCV_FOUND )
        TARGET_LINK_LIBRARIES( OrcaLogFactory OrcaNavUtil )
    ENDIF ( OPENCV_FOUND )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( NOT OS_WIN )
