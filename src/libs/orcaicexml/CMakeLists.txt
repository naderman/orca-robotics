SET ( lib_name OrcaIceXml )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
REQUIRE_VAR( build LIB ${lib_name} ORCA_OS_LINUX "only Linux OS is supported" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} IceXML )

    INSTALL_FILES( /include/orcaicexml .*\\.h$ )

ENDIF ( build )
