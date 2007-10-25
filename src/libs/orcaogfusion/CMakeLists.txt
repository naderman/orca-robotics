IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaOgFusion SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( OrcaOgFusion OrcaOgMap  )
    
    INSTALL_FILES( /include/orcaogfusion .*\\.h$ )

ENDIF ( NOT OS_WIN )