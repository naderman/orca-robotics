IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaWall SHARED ${SRCS} )
    
    INSTALL_FILES( /include/orcawall .*\\.h$ ) 

ENDIF ( NOT OS_WIN )
