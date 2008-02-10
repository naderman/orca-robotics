SET ( lib_name OrcaOgMap )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

SET( dep_libs HydroOgMap )
REQUIRE_VAR( build LIB ${lib_name} HYDROOGMAP_INSTALLED "libHydroOgMap not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcaogmap .*\\.h$ )

    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF (build )
