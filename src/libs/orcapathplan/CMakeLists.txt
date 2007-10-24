IF ( OS_LINUX )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    LINK_LIBRARIES( OrcaOgMap OrcaMisc )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaPathPlan SHARED ${SRCS} )

    INSTALL_FILES( /include/orcapathplan .*\\.h$ )
    
    IF( BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( BUILD_TESTS )       

ENDIF ( OS_LINUX )
