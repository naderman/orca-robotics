ORCA_SET_LIBRARY_NAME( OrcaSystemStatusUtil )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( int_libs OrcaIfaceImpl ) 
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${int_libs} )

if( build )

    # now we use all of orcaice for connections
    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    set( dep_libs ${ext_libs} ${int_libs} ${other_libs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )
