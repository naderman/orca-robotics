SET( lib_name OrcaQt )
GBX_ADD_LICENSE( GPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
SET( not_os_win NOT GBX_OS_WIN )
GBX_REQUIRE_VAR( build LIB ${lib_name} not_os_win "Windows OS is not supported" )
GBX_REQUIRE_VAR( build LIB ${lib_name} QT4_FOUND "Qt4 not found" )

IF( build )

    INCLUDE( ${HYDRO_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB human_srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${lib_name} SHARED ${human_srcs} )
        
    GBX_ADD_HEADERS( orcaqt ${hdrs} )

ENDIF( build )
