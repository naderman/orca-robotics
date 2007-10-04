IF ( OS_LINUX )

OPTION( BUILD_ORCAICEXML "Try to build libOrcaIceXml" ON )

IF ( BUILD_ORCAICEXML )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaIceXml SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaIceXml IceXML )

    INSTALL_FILES( /include/orcaicexml .*\\.h$ )

ENDIF ( BUILD_ORCAICEXML )

ENDIF ( OS_LINUX )

