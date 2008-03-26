SET ( lib_name OrcaOgMap )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )

SET( dep_libs HydroOgMap )
GBX_REQUIRE_VAR( build LIB ${lib_name} HYDROOGMAP_INSTALLED "libHydroOgMap not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    GBX_ADD_HEADERS( orcaogmap ${hdrs} )

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF (build )
