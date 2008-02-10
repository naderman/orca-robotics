SET ( lib_name OrcaQUml )
GLOBAL_ADD_LICENSE( GPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_ORCA_OS_WIN NOT ORCA_OS_WIN )
REQUIRE_VAR( build LIB ${lib_name} NOT_ORCA_OS_WIN "Windows OS is not  supported" )
REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaDef )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB human_srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} )
    
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )

ENDIF ( build )
