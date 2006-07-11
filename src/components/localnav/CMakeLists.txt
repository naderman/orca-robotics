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
    
    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )
    
    # Files for the icebox service
    SET( SERVICE_SRCS serviceI ${COMMON_SRCS} )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( localnav ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( localnav vfhdriver OrcaNavUtil )

    # build an IceBox service
    ADD_LIBRARY( OrcaLocalNav SHARED ${SERVICE_SRCS} )
    INSTALL_TARGETS( /lib OrcaLocalNav )
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaLocalNav vfhdriver OrcaNavUtil )

    GENERATE_FROM_DEF( localnav.def )

ENDIF ( NOT WIN32 AND BUILD_LOCALNAV )
