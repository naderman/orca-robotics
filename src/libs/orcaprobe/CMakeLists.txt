SET( lib_name OrcaProbe )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not  supported" )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    
    GBX_ADD_HEADERS( orcaprobe ${hdrs} )

ENDIF( build )
