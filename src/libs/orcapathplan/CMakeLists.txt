SET ( lib_name OrcaPathPlan )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
REQUIRE_VAR( build LIB ${lib_name} ORCA_OS_LINUX "only Linux OS is supported" )

SET( dep_libs ${dep_libs} HydroPathPlan OrcaObjects )
REQUIRE_VAR( build LIB ${lib_name} HYDROPATHPLAN_INSTALLED "libHydroPathPlan not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )
    INSTALL_FILES( /include/orcapathplan .*\\.h$ )
    
ENDIF ( build )
