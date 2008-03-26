SET ( lib_name OrcaObjectUtil )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaObjects )
    
    GBX_ADD_HEADERS( orcaobjutil ${hdrs} )

ENDIF ( build )
