SET ( LIB_NAME OrcaLogFactory )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

SET( DEP_LIBS OrcaLog OrcaIfaceImpl )
REQUIRE_TARGETS( BUILD LIB ${LIB_NAME} ${DEP_LIBS} )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )

# camera logging is broken. all of image stuff requires a major overhaul
#     INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
#     IF ( OPENCV_FOUND )
#         INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)
#         ADD_DEFINITIONS( -DOPENCV_FOUND )
#         SET( DEP_LIBS ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
#     ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEP_LIBS} )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )
    
ENDIF ( BUILD )
