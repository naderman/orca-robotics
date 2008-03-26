SET ( lib_name OrcaIceXml )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
GBX_REQUIRE_VAR( build LIB ${lib_name} GBX_OS_LINUX "only Linux OS is supported" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} IceXML )

    GBX_ADD_HEADERS( orcaicexml ${hdrs} )

ENDIF ( build )
