ORCA_SET_LIBRARY_NAME( OrcaOgFusion )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} not_os_win "Windows OS is not  supported" )

SET( dep_libs OrcaOgMap )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs}  )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )