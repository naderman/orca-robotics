OPTION( BUILD_FAITHLOCALISER "Try to build FaithLocaliser component" ON )

IF ( NOT WIN32 AND BUILD_FAITHLOCALISER )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS
        component mainloop localise2dI position2dconsumerI
    )

    # Files for the component
    SET( COMP_SRCS main  ${COMMON_SRCS} )
    
    # stand-alone component  
    GLOBAL_ADD_EXECUTABLE( faithlocaliser ${COMP_SRCS} )

    # Files for the service
    SET( SERV_SRCS  service ${COMMON_SRCS} )
    
    # build an IceBox service
    ADD_LIBRARY( OrcaFaithLocaliser SHARED ${SERV_SRCS} )
    
    INSTALL_TARGETS( /lib OrcaFaithLocaliser )
    
    GENERATE_FROM_DEF( faithlocaliser.def )

ENDIF ( NOT WIN32 AND BUILD_FAITHLOCALISER )
