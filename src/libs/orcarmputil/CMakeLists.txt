ORCA_SET_LIBRARY_NAME( OrcaRmpUtil )
GBX_ADD_LICENSE( LGPL )

set( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

set( dep_libs HydroRobotDriverUtil )
GBX_REQUIRE_LIBS( build LIB ${LIB_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    include_directories( 
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    file( GLOB hdrs *.h )
    file( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${LIB_VERSION} ${srcs} )
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

    target_link_libraries( ${LIB_NAME} ${dep_libs} )

    add_subdirectory( util )

endif( build )
