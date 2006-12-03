OPTION( BUILD_FAITHLOCALISER "Try to build FaithLocaliser component" ON )

IF ( NOT WIN32 AND BUILD_FAITHLOCALISER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
        component mainloop localise2dI position2dconsumerI
    )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( faithlocaliser main ${COMMON_SRCS} )
    
    # build an IceBox service
    GLOBAL_ADD_LIBRARY( OrcaFaithLocaliser SHARED service ${COMMON_SRCS} )
    
    GENERATE_CONFIG_FILE( faithlocaliser.def )

ENDIF ( NOT WIN32 AND BUILD_FAITHLOCALISER )
