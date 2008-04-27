ORCA_SET_LIBRARY_NAME( OrcaCompModel )
SET( LIB_INSTALL orcacm )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
        TARGET_LINK_LIBRARIES( ${LIB_NAME} IceGrid )
    ELSE ( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${LIB_NAME} STATIC ${srcs} )
        TARGET_LINK_LIBRARIES( ${LIB_NAME}
                    optimized IceGrid debug IceGridd
                )
    ENDIF( NOT GBX_OS_WIN )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaInterfaces )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
