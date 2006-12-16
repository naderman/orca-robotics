OPTION( BUILD_LOCALNAV "Try to build LocalNav component" ON )

IF ( NOT WIN32 AND NOT QNXNTO )
IF ( BUILD_LOCALNAV )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
      ${PROJECT_SOURCE_DIR}/src/components/localnav
      )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component goalwatcher localnavdriver localnavmanager mainloop pathfollower2dI pathmaintainer )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( localnav main ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( localnav OrcaLocalNavVfh OrcaNavUtil )

    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLocalNav SHARED service ${COMMON_SRCS} )

    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaLocalNav OrcaLocalNavVfh OrcaNavUtil )

    GENERATE_CONFIG_FILE( localnav.def )

ENDIF ( BUILD_LOCALNAV )
ENDIF ( NOT WIN32 AND NOT QNXNTO )
