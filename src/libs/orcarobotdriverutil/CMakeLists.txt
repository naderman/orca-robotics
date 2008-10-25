ORCA_SET_LIBRARY_NAME( OrcaRobotDriverUtil )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )

ENDIF( build )
