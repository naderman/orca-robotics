INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_LIBRARY( OrcaIfaceImpl SHARED ${SRCS} )

INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
