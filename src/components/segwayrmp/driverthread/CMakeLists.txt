ORCA_SET_LIBRARY_NAME( SegwayRmpDriverThread )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs  OrcaRobotDriverUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
    
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    add_subdirectory( util )

endif( build )
