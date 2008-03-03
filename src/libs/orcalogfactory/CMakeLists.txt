SET ( lib_name OrcaLogFactory )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaLog OrcaIfaceImpl OrcaObjects )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

# camera logging is broken. all of image stuff requires a major overhaul
#     INCLUDE (${ORCA_CMAKE_DIR}/FindOpenCV.cmake)
#     IF ( OPENCV_FOUND )
#         INCLUDE (${ORCA_CMAKE_DIR}/UseOpenCV.cmake)
#         ADD_DEFINITIONS( -DOPENCV_FOUND )
#         SET( dep_libs ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
#     ENDIF ( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcalogfactory .*\\.h$ ) 

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )
    
ENDIF ( build )
