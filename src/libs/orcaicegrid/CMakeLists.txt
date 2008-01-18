SET ( LIB_NAME OrcaIceGrid )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )

    INSTALL_FILES( /include/orcaicegrid .*\\.h$ )

ENDIF ( BUILD )
