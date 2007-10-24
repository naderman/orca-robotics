IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaProbeFactory SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( OrcaProbeFactory OrcaProbe )
    
ENDIF ( NOT OS_WIN )
