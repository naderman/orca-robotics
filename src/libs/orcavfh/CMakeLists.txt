SET( lib_name OrcaVfh )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( int_libs OrcaLocalNav )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${int_libs} )

SET( dep_libs ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
     
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

#    GBX_ADD_HEADERS( orcavfh ${hdrs} )

ENDIF( build )
