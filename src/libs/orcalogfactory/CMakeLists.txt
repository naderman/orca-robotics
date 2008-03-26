SET ( lib_name OrcaLogFactory )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaLog OrcaIfaceImpl OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

# camera logging is broken. all of image stuff requires a major overhaul
#     INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
#     IF ( OPENCV_FOUND )
#         INCLUDE( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
#         ADD_DEFINITIONS( -DOPENCV_FOUND )
#         SET( dep_libs ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
#     ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    GBX_ADD_HEADERS( orcalogfactory ${hdrs} )   

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )
    
ENDIF ( build )
