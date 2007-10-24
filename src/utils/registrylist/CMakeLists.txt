OPTION( BUILD_REGISTRY_LIST "Try to build RegistryList component" ON )

IF ( BUILD_REGISTRY_LIST )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB SRCS *.cpp )
    
    GLOBAL_ADD_EXECUTABLE( registrylist ${SRCS} )
        
    IF ( NOT OS_WIN )
        TARGET_LINK_LIBRARIES( registrylist IceGrid OrcaCompModel OrcaMisc )
    ELSE ( NOT OS_WIN )
        TARGET_LINK_LIBRARIES( registrylist 
              optimized IceGrid debug IceGridd
              optimized Glacier2 debug Glacier2d
              OrcaCompModel OrcaMisc )
    ENDIF ( NOT OS_WIN )

    GENERATE_CONFIG_FILE( registrylist.def )

ENDIF ( BUILD_REGISTRY_LIST )
