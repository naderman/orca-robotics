SET ( lib_name OrcaImage )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} OFF )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

# this lib should be split up into sublibraries with simpler dependencies
# INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
# REQUIRE_VAR( build LIB ${lib_name} OPENCV_FOUND "OpenCV not found" )

IF ( build )

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
        TARGET_LINK_LIBRARIES( ${lib_name} triclops )
    ENDIF ( TRICLOPS_FOUND )
    

    FILE( GLOB srcs *.cpp )
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

    INSTALL_FILES( /include/orcaimage .*\\.h$ )

ENDIF ( build )
