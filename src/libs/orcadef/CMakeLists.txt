SET( lib_name OrcaDef )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( dep_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaObjects )
    
    GBX_ADD_HEADERS( orcadef ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
