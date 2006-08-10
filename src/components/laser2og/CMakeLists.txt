OPTION( BUILD_LASER2OG "Try to build Laser2Og" OFF )

IF ( NOT WIN32 )
IF ( BUILD_LASER2OG )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/utils )
    
    GLOBAL_ADD_EXECUTABLE( laser2og ${SRCS} )
    TARGET_LINK_LIBRARIES( laser2og OrcaOgFusion OrcaPathPlan OrcaMisc )
    GENERATE_CONFIG_FILE( laser2og.def )

ENDIF ( BUILD_LASER2OG )
ENDIF ( NOT WIN32 )
