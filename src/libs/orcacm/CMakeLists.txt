SET( lib_name OrcaCompModel )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
        TARGET_LINK_LIBRARIES( ${lib_name} IceGrid )
    ELSE ( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
        TARGET_LINK_LIBRARIES( ${lib_name}
                    optimized IceGrid debug IceGridd
                )
    ENDIF( NOT GBX_OS_WIN )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    GBX_ADD_HEADERS( orcacm ${hdrs} )

ENDIF( build )
