ORCA_SET_LIBRARY_NAME( OrcaLogFactory )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )

SET( int_libs OrcaLog OrcaIfaceImpl OrcaObjects OrcaIfaceLog )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )

# camera logging is broken. all of image stuff requires a major overhaul
#     INCLUDE( ${ORCA_CMAKE_DIR}/FindOpenCV.cmake )
#     IF( OPENCV_FOUND )
#         INCLUDE( ${ORCA_CMAKE_DIR}/UseOpenCV.cmake )
#         ADD_DEFINITIONS( -DOPENCV_FOUND )
#         SET( dep_libs ${COMP_DEPS} ${OPENCV_LIBS} OrcaImage )
#     ENDIF( OPENCV_FOUND )

    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )   

    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )
    
ENDIF( build )
