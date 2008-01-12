SET ( LIB_NAME OrcaCompModel )
GLOBAL_ADD_LICENSE( LGPL )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
    
    IF ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} IceGrid )
    ELSE ( NOT OS_WIN )
        GLOBAL_ADD_LIBRARY( ${LIB_NAME} STATIC ${SRCS} )
        TARGET_LINK_LIBRARIES( ${LIB_NAME}
                    optimized IceGrid debug IceGridd
                )
    ENDIF ( NOT OS_WIN )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaInterfaces )
    
    INSTALL_FILES( /include/orcacm .*\\.h$ )

ENDIF ( BUILD )
