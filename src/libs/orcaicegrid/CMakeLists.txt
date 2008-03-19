SET ( lib_name OrcaIceGrid )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
REQUIRE_VAR( build LIB ${lib_name} ORCA_OS_LINUX "only Linux OS is supported" )

IF ( build )

    INCLUDE( ${HYDRO_CMAKE_DIR}/UseGearbox.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

    INSTALL_FILES( /include/orcaicegrid .*\\.h$ )

ENDIF ( build )
