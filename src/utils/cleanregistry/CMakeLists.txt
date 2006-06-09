OPTION( BUILD_CLEAN_REGISTRY "Try to build ClearRegistry Utility" ON )

IF ( BUILD_CLEAN_REGISTRY )

    INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJ_SOURCE_DIR}/config/include_orcaice.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )

    INCLUDE_DIRECTORIES(
        ${PROJ_SOURCE_DIR}/src/utils
    )

    LINK_DIRECTORIES(
        ${PROJ_BINARY_DIR}/src/utils/orcacm
    )
    
    GLOBAL_ADD_EXECUTABLE( cleanregistry ${SRCS} )

    TARGET_LINK_LIBRARIES ( cleanregistry OrcaCompModel OrcaMisc IceGrid )
    
    GENERATE_FROM_DEF( cleanregistry.def )

ENDIF ( BUILD_CLEAN_REGISTRY )
