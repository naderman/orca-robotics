SET( lib_name OrcaWallFactory )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )

SET( dep_libs OrcaWall OrcaIfaceImpl OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs}  )
    
ENDIF( build )
