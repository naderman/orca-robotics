SET ( lib_name OrcaBros1 )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
REQUIRE_VAR( build LIB ${lib_name} ORCA_OS_LINUX "only Linux OS is supported" )

SET( dep_libs OrcaInterfaces )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

SET( dep_libs ${dep_libs} HydroBros1 )
REQUIRE_VAR( build LIB ${lib_name} HYDROBROS1_INSTALLED "libHydroBros1 not installed" )

IF ( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydro.cmake )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED convertutils.cpp )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcabros1 .*\\.h$ )

ENDIF ( build )
