ORCA_SET_LIBRARY_NAME( HydroBros1 )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

if( build )

    include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    GBX_ADD_HEADERS( hydrobros1 ${hdrs} )

endif( build )
