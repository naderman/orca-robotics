ORCA_SET_LIBRARY_NAME( SegwayRmpDriverThread )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

SET( int_libs  OrcaRobotDriverUtil )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
#    INCLUDE_DIRECTORIES( 
#        ${PROJECT_SOURCE_DIR}/src/libs
#    )
    
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} )

    ADD_SUBDIRECTORY( util )

ENDIF( build )
