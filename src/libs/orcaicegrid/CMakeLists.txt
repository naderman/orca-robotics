INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )

FILE( GLOB SRCS *.cpp )

GLOBAL_ADD_LIBRARY( OrcaIceGrid SHARED ${SRCS} )

INSTALL_FILES( /include/orcaicegrid .*\\.h$ )
