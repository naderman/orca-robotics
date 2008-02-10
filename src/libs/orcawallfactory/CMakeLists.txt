SET ( lib_name OrcaWallFactory )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )

SET( dep_libs OrcaWall OrcaIfaceImpl OrcaObjects )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs}  )
    
ENDIF ( build )
