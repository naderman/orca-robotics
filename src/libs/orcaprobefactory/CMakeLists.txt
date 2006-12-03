IF ( NOT WIN32 )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaProbeFactory SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( OrcaProbeFactory OrcaProbe )
    
ENDIF ( NOT WIN32 )
