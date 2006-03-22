OPTION( BUILD_LOCALNAV "Try to build LocalNav component" ON )

IF ( BUILD_LOCALNAV )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

    SUBDIRS( vfhdriver )
    MESSAGE( "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
      ${PROJ_SOURCE_DIR}/src/components/localnav
      ${PROJ_SOURCE_DIR}/src/utils
      )

    LINK_DIRECTORIES(
      ${PROJ_BINARY_DIR}/src/components/localnav/vfhdriver
      ${PROJ_BINARY_DIR}/src/utils/orcanavutil
      )

    FILE( GLOB SRCS *.cpp )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( localnav ${SRCS} )
    TARGET_LINK_LIBRARIES( localnav vfhdriver OrcaNavUtil )

    GENERATE_FROM_DEF( localnav.def )

ENDIF ( BUILD_LOCALNAV )
