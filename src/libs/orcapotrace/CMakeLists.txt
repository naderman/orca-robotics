ORCA_SET_LIBRARY_NAME( OrcaPotrace )
GBX_ADD_LICENSE( GPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs HydroOgMap )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

GBX_REQUIRE_VAR( build LIB ${LIB_NAME} POTRACE_FOUND "Potrace not found" )
set( dep_libs ${dep_libs} potrace )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif(build )
