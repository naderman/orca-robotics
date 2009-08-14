ORCA_SET_LIBRARY_NAME( HydroMapLoad )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

GBX_REQUIRE_VAR( build LIB ${LIB_NAME} QT4_FOUND "Qt4 not found" )

set( dep_libs HydroOgMap )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${GEARBOX_USE_FILE} )
    include( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( hydromapload ${hdrs} )

    add_subdirectory( test )

endif( build )
