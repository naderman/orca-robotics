SET ( lib_name OrcaQUml )
GBX_ADD_LICENSE( GPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET ( NOT_GBX_OS_WIN NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} NOT_GBX_OS_WIN "Windows OS is not  supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

SET( dep_libs OrcaDef )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB human_srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} )
    
    TARGET_LINK_LIBRARIES ( ${lib_name} ${dep_libs} )

ENDIF ( build )
