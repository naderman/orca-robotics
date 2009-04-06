ORCA_SET_LIBRARY_NAME( OrcaObjects )
set( LIB_INSTALL orcaobj )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
# This is one of fundamental libs. It is not optional (but maybe it should change)

# these 2 lines just check prereq's, but linking is specified in the macros below.
set( dep_libs GbxUtilAcfr GbxIceUtilAcfr HydroIceUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceUtil.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
    endif( ORCA_BUILD_TESTS )

endif( build )
