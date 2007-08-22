OPTION( BUILD_PINGSERVER "Try to build PingServer component" ON )

IF ( BUILD_PINGSERVER )

  IF ( NOT LIBOPING_FOUND )
    MESSAGE( STATUS "Not Planning to Build Component: pingserver -- liboping not found")
  ELSE ( NOT LIBOPING_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    FILE ( GLOB SRCS *.cpp )

    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pingserver ${SRCS} )
    TARGET_LINK_LIBRARIES( pingserver OrcaPing )

    GENERATE_CONFIG_FILE( pingserver.def )

  ENDIF ( NOT LIBOPING_FOUND )

ENDIF ( BUILD_PINGSERVER )
