IF ( NOT WIN32 )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )
    
    GLOBAL_ADD_LIBRARY( OrcaLogFactory SHARED ${SRCS} )
    
    # Need OpenCV to compress images to file
    IF ( OPENCV_FOUND )
        INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs ${OPENCV_INCLUDE_DIRS} )
        TARGET_LINK_LIBRARIES( OrcaLogFactory OrcaNavUtil ${OPENCV_LIBS} OrcaImage )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ELSE ( OPENCV_FOUND )
        TARGET_LINK_LIBRARIES( OrcaLogFactory OrcaNavUtil )
    ENDIF ( OPENCV_FOUND )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( NOT WIN32 )
