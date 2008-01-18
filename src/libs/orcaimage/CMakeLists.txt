SET ( LIB_NAME OrcaImage )
GLOBAL_ADD_LICENSE( GPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )

# this lib should be split up into sublibraries with simpler dependencies
# INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
# REQUIRE_VAR( BUILD LIB ${LIB_NAME} OPENCV_FOUND "OpenCV not found" )

IF ( BUILD )

    # alexm: this seems like a big dependency
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
    IF ( OPENCV_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
        ADD_DEFINITIONS( -DOPENCV_FOUND )
    ENDIF ( OPENCV_FOUND )

    # check for digiclops image grabbing libraries
    FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
    ASSERT( TRICLOPS_FOUND "Looking for Triclops - not found" "Looking for Triclops - found" )

    IF ( TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        ADD_DEFINITIONS( -DTRICLOPS_FOUND )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} triclops )
    ENDIF ( TRICLOPS_FOUND )
    

    FILE( GLOB SRCS *.cpp )
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )

    INSTALL_FILES( /include/orcaimage .*\\.h$ )

ENDIF ( BUILD )
