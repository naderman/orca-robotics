IF ( OS_LINUX )

OPTION( BUILD_ORCAICEGRID "Try to build libOrcaIceGrid" ON )

IF ( BUILD_ORCAICEGRID )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaIceGrid SHARED ${SRCS} )

    INSTALL_FILES( /include/orcaicegrid .*\\.h$ )

ENDIF ( BUILD_ORCAICEGRID )

ENDIF ( OS_LINUX )

