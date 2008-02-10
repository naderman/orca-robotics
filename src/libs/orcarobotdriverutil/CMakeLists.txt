SET ( lib_name OrcaRobotDriverUtil )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    INCLUDE_DIRECTORIES( 
        ${PROJECT_BINARY_DIR}
        ${PROJECT_SOURCE_DIR}/src/libs
    )
    
    FILE( GLOB srcs *.cpp )
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    INSTALL_FILES( /include/orcarobotdriverutil .*\\.h$ )

ENDIF ( build )
