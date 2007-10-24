IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaProbe SHARED ${SRCS} )
    
    INSTALL_FILES( /include/orcaprobe .*\\.h$ ) 

ENDIF ( NOT OS_WIN )
