ORCA_SET_LIBRARY_NAME( OrcaPotrace )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

SET( dep_libs HydroOgMap )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} HYDROOGMAP_INSTALLED "libHydroOgMap not installed" )

GBX_REQUIRE_VAR( build EXE ${LIB_NAME} POTRACE_FOUND "Potrace not found" )
SET( ext_libs potrace )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} ${ext_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF (build )
