SET ( lib_name OrcaRobotDriverUtil )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )

    GBX_ADD_HEADERS( orcarobotdriverutil ${hdrs} )

ENDIF ( build )
