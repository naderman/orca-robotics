INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

FILE( GLOB SRCS *.cpp )

IF ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaIfaceImpl SHARED ${SRCS} )
ELSE ( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( OrcaIfaceImpl STATIC ${SRCS} )
ENDIF ( NOT OS_WIN )

INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
