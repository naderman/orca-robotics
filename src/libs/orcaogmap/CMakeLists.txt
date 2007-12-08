SET ( LIB_NAME OrcaOgMap )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( LIB ${LIB_NAME} NOT_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( LIB ${LIB_NAME} HYDROOGMAP_INSTALLED "libHydroOgMap not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} HydroOgMap )

    INSTALL_FILES( /include/orcaogmap .*\\.h$ )

    IF ( BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( BUILD_TESTS )

ENDIF (BUILD )
