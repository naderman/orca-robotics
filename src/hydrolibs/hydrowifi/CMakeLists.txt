ORCA_SET_LIBRARY_NAME( HydroWifi )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "only Linux OS is supported" )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    if( ORCA_BUILD_TESTS ) 
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )


