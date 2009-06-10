ORCA_SET_LIBRARY_NAME( OrcaIceXml )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "only Linux OS is supported" )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    target_link_libraries( ${LIB_NAME} IceXML )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

endif( build )
