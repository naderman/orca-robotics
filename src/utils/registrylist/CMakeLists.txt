OPTION( BUILD_REGISTRY_LIST "Try to build RegistryList component" ON )

IF ( BUILD_REGISTRY_LIST )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( registrylist ${SRCS} )
        
    IF ( NOT WIN32 )
        TARGET_LINK_LIBRARIES( registrylist IceGrid OrcaCompModel OrcaMisc )
    ELSE ( NOT WIN32 )
        TARGET_LINK_LIBRARIES( registrylist 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaCompModel OrcaMisc )
    ENDIF ( NOT WIN32 )

    GENERATE_CONFIG_FILE( registrylist.def )

ENDIF ( BUILD_REGISTRY_LIST )
