SET ( LIB_NAME OrcaBros1 )
GLOBAL_ADD_LICENSE( LGPL )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
REQUIRE_VAR( LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )
REQUIRE_VAR( LIB ${LIB_NAME} HYDROBROS1_INSTALLED "libHydroBros1 not installed" )

IF ( BUILD )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydro.cmake )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED convertutils.cpp )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} HydroBros1 OrcaInterfaces )

    INSTALL_FILES( /include/orcabros1 .*\\.h$ )

ENDIF ( BUILD )
