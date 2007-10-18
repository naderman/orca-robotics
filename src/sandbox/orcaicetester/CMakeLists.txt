IF ( NOT OS_WIN AND BUILD_TESTS )

  OPTION( BUILD_ORCAICE_TESTER "Try to build a Tester for standard libOrcaIce interfaces" ON )

  IF ( BUILD_ORCAICE_TESTER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( orcaicetester ${SRCS} )

    GENERATE_CONFIG_FILE( orcaicetester.def )

  ENDIF ( BUILD_ORCAICE_TESTER )

ENDIF ( NOT OS_WIN AND BUILD_TESTS )

