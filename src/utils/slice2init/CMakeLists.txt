ORCA_SET_COMPONENT_NAME( Slice2Init )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    include( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

    file( GLOB srcs *.cpp )
    set( dep_libs IceUtil Slice )
    
    GBX_ADD_EXECUTABLE( ${APP_NAME} ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )

   if( ORCA_BUILD_TESTS )
       add_subdirectory( test )
   endif( ORCA_BUILD_TESTS )

endif( build )
