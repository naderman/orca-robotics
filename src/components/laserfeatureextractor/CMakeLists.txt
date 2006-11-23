OPTION( BUILD_LASERFEATUREEXTRACTOR "Try to build LaserFeatureExtractor" ON )

IF ( NOT WIN32 AND BUILD_LASERFEATUREEXTRACTOR )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )
    
    FILE( GLOB SRCS *.cpp )
    # Set LIB_SRCS to everything _except_ main.cpp
    SET( LIB_SRCS ${SRCS} )
    LIST( REMOVE_ITEM LIB_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" )
    # Set COMP_SRCS to everything _except_ service.cpp
    SET( COMP_SRCS ${SRCS} )
    LIST( REMOVE_ITEM COMP_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/service.cpp" )

    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( laserfeatureextractor ${COMP_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLaserFeatureExtractor SHARED ${LIB_SRCS} )

    GENERATE_CONFIG_FILE( laserfeatureextractor.def )

ENDIF ( NOT WIN32 AND BUILD_LASERFEATUREEXTRACTOR )
