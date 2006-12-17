IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaProbeFactory SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( OrcaProbeFactory OrcaProbe )
    
ENDIF ( NOT OS_WIN )
