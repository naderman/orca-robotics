IF ( NOT OS_WIN )
IF ( QT4_FOUND ) 

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaMapLoad SHARED ${SRCS} )

    TARGET_LINK_LIBRARIES ( OrcaMapLoad OrcaOgMap )

    INSTALL_FILES( /include/orcamapload .*\\.h$ )

ENDIF ( QT4_FOUND )
ENDIF ( NOT OS_WIN  )
