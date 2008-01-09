SET ( LIB_NAME OrcaLogFactory )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )

    SET( COMP_DEPS OrcaLog OrcaIfaceImpl )

# camera logging is broken. all of image stuff requires a major overhaul
#     INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
#     IF ( OPENCV_FOUND )
#         INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)
#         ADD_DEFINITIONS( -DOPENCV_FOUND )
#         SET( COMP_DEPS ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
#     ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${COMP_DEPS} )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( BUILD )
