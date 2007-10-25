IF ( OS_LINUX  )

OPTION( BUILD_LASER2OG "Try to build Laser2Og" ON )

IF ( BUILD_LASER2OG )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( laser2og ${SRCS} )
    TARGET_LINK_LIBRARIES( laser2og OrcaOgFusion OrcaPathPlan  )
    GENERATE_CONFIG_FILE( laser2og.def )

ENDIF ( BUILD_LASER2OG )
ENDIF ( OS_LINUX )

