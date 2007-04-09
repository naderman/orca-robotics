IF ( NOT OS_WIN )

OPTION( BUILD_NOISYROBOT2D "Try to build NoisyRobot2d component" ON )

IF ( BUILD_NOISYROBOT2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( NoisyRobot2dSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( noisyrobot2d main )
    TARGET_LINK_LIBRARIES( noisyrobot2d NoisyRobot2dSources OrcaNavUtil OrcaMisc )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaNoisyRobot2d SHARED service )
    TARGET_LINK_LIBRARIES( OrcaNoisyRobot2d NoisyRobot2dSources OrcaNavUtil OrcaMisc )

    GENERATE_CONFIG_FILE( noisyrobot2d.def )

ENDIF ( BUILD_NOISYROBOT2D )
ENDIF ( NOT OS_WIN )
