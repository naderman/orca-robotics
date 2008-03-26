SET( lib_name OrcaBros1 )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
GBX_REQUIRE_VAR( build LIB ${lib_name} GBX_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroBros1 )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

SET( int_libs OrcaInterfaces )
GBX_REQUIRE_TARGETS( build LIB ${lib_name} ${int_libs} )

IF( build )
    
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydro.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} )

    GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    GBX_ADD_HEADERS( orcabros1 ${hdrs} )

ENDIF( build )
