SET ( lib_name OrcaCompModel )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    IF ( NOT ORCA_OS_WIN )
        GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
        TARGET_LINK_LIBRARIES( ${lib_name} IceGrid )
    ELSE ( NOT ORCA_OS_WIN )
        GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
        TARGET_LINK_LIBRARIES( ${lib_name}
                    optimized IceGrid debug IceGridd
                )
    ENDIF ( NOT ORCA_OS_WIN )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    INSTALL_FILES( /include/orcacm .*\\.h$ )

ENDIF ( build )
