SET( lib_name OrcaImage )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 Not Found" )

# this lib should be split up into sublibraries with simpler dependencies
# INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
# GBX_REQUIRE_VAR( build LIB ${lib_name} OPENCV_FOUND "OpenCV not found" )

IF( build )

    # alexm: this seems like a big dependency
    # not sure what is actually needed.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
    IF( OPENCV_FOUND )
        INCLUDE( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
        ADD_DEFINITIONS( -DUSE_OPENCV )
    ENDIF( OPENCV_FOUND )




    # check for digiclops image grabbing libraries
    FIND_LIBRARY( TRICLOPS_FOUND NAMES triclops PATHS /usr/lib /usr/local/lib )
    ASSERT( TRICLOPS_FOUND "Looking for Triclops - not found" "Looking for Triclops - found" )

    IF( TRICLOPS_FOUND )
        INCLUDE_DIRECTORIES( /usr/local/include/triclops )
        ADD_DEFINITIONS( -DTRICLOPS_FOUND )
        TARGET_LINK_LIBRARIES( ${lib_name} triclops )
    ENDIF( TRICLOPS_FOUND )

    #IF( QT_FOUND )
    #    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )
    #    ADD_DEFINITIONS( -DUSE_QT )
    #ENDIF( QT_FOUND )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

    GBX_ADD_HEADERS( orcaimage ${hdrs} )

ENDIF( build )
