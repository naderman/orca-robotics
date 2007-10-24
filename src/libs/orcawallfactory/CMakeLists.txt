IF ( NOT OS_WIN )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_LIBRARY( OrcaWallFactory SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( OrcaWallFactory OrcaWall OrcaIfaceImpl OrcaMisc )
    
ENDIF ( NOT OS_WIN )
