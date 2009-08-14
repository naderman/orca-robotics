ORCA_SET_LIBRARY_NAME( OrcaIce )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
# This is one of fundamental libs. It is not optional.

set( dep_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    # these are the same as in UseComponentRules but without project rules
    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    include( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    include( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )
    # debug only! comment out when finished.
#     include( ${ORCA_CMAKE_DIR}/UseOrcaIfaceUtil.cmake )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp detail/*.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    
    target_link_libraries( ${LIB_NAME} ${dep_libs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    if( ORCA_BUILD_TESTS )
        add_subdirectory( test )
        add_subdirectory( detail/test )
    endif( ORCA_BUILD_TESTS )

endif( build )
