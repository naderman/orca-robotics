SET ( LIB_NAME OrcaPathPlan )
GLOBAL_ADD_LICENSE( LGPL )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
REQUIRE_VAR( LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )
REQUIRE_VAR( LIB ${LIB_NAME} HYDROPATHPLAN_INSTALLED "libHydroPathPlan not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} HydroPathPlan )
    INSTALL_FILES( /include/orcapathplan .*\\.h$ )
    
ENDIF ( BUILD )
