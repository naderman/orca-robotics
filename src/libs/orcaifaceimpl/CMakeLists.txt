SET( lib_name OrcaIfaceImpl )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE ( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF( NOT GBX_OS_WIN )

    TARGET_LINK_LIBRARIES( ${lib_name} OrcaObjects )
    
    GBX_ADD_HEADERS( orcaifaceimpl ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )