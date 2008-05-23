ORCA_SET_LIBRARY_NAME( OrcaIfaceImpl )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceString.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
    ELSE ( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${LIB_NAME} STATIC ${srcs} )
    ENDIF( NOT GBX_OS_WIN )

    TARGET_LINK_LIBRARIES( ${LIB_NAME} OrcaObjects )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )