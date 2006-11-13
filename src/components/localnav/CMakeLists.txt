OPTION( BUILD_LOCALNAV "Try to build LocalNav component" ON )

IF ( NOT WIN32 AND BUILD_LOCALNAV )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
      ${PROJECT_SOURCE_DIR}/src/components/localnav
      ${PROJECT_SOURCE_DIR}/src/utils
      )

    LINK_DIRECTORIES(
      ${PROJECT_BINARY_DIR}/src/components/localnav/vfhdriver
      ${PROJECT_BINARY_DIR}/src/utils/orcanavutil
      )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component goalwatcher localnavdriver localnavmanager mainloop pathfollower2dI pathmaintainer )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( localnav main ${COMMON_SRCS} )
    TARGET_LINK_LIBRARIES( localnav vfhdriver OrcaNavUtil )

    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLocalNav SHARED service ${COMMON_SRCS} )

    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaLocalNav OrcaLocalNavVfh OrcaNavUtil )

    GENERATE_CONFIG_FILE( localnav.def )

ENDIF ( NOT WIN32 AND BUILD_LOCALNAV )
