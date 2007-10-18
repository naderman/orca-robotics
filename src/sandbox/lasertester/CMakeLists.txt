IF ( BUILD_TESTS )

  OPTION( BUILD_LASER_TESTER "Try to build Laser Tester" ON )

  IF ( NOT OS_WIN AND BUILD_LASER_TESTER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_EXECUTABLE( lasertester ${SRCS} )

    GENERATE_CONFIG_FILE( lasertester.def )
    
  ENDIF ( NOT OS_WIN AND BUILD_LASER_TESTER )

ENDIF ( BUILD_TESTS )
