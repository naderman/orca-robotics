ORCA_SET_LIBRARY_NAME( OrcaObjectUtil )
SET( LIB_INSTALL orcaobjutil )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GBX_ADD_LIBRARY( ${LIB_NAME} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaObjects )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
