INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

FILE( GLOB SRCS *.cpp )

IF ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaIfaceImpl SHARED ${SRCS} )
ELSE ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaIfaceImpl STATIC ${SRCS} )
ENDIF ( NOT OS_WIN )

INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
