IF ( NOT WIN32 )

  INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

  FILE( GLOB SRCS *.cpp )

  GLOBAL_ADD_LIBRARY( OrcaOgFusion SHARED ${SRCS} )

  INSTALL_FILES( /include/orcaogfusion .*\\.h$ )

ENDIF ( NOT WIN32 )