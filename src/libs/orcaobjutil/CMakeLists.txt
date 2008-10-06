ORCA_SET_LIBRARY_NAME( OrcaObjectUtil )
SET( LIB_INSTALL orcaobjutil )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

SET( int_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} )
    
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GBX_ADD_LIBRARY( ${LIB_NAME} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)

    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
