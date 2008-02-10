SET ( lib_name OrcaQt )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_srcs *.cpp )
    
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${HUMAN_srcs} )
        
    INSTALL_FILES( /include/orcaqt .*\\.h$ )

ENDIF ( build )
