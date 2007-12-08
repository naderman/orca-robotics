SET ( LIB_NAME OrcaIceXml )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
REQUIRE_VAR( LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} IceXML )

    INSTALL_FILES( /include/orcaicexml .*\\.h$ )

ENDIF ( BUILD )
