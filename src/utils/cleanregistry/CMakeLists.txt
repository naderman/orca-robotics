OPTION( BUILD_CLEAN_REGISTRY "Try to build ClearRegistry Utility" ON )

IF ( NOT OS_WIN AND BUILD_CLEAN_REGISTRY )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )

    INCLUDE_DIRECTORIES(
        ${PROJECT_SOURCE_DIR}/src/utils
    )

    LINK_DIRECTORIES(
        ${PROJECT_BINARY_DIR}/src/utils/orcacm
    )
    
    GLOBAL_ADD_EXECUTABLE( cleanregistry ${SRCS} )

    TARGET_LINK_LIBRARIES ( cleanregistry OrcaCompModel OrcaMisc IceGrid )
    
    GENERATE_CONFIG_FILE( cleanregistry.def )

ENDIF ( NOT OS_WIN AND BUILD_CLEAN_REGISTRY )
