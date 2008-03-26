SET ( lib_name OrcaLocalNav )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${lib_name} ON )
GBX_REQUIRE_VAR( build LIB ${lib_name} GBX_OS_LINUX "only Linux OS is supported" )

SET( dep_libs HydroNavUtil OrcaObjects )
GBX_REQUIRE_VAR( build LIB ${lib_name} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

     INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       ${PROJECT_SOURCE_DIR}/src/components/localnav
       )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )

    GBX_ADD_LIBRARY( ${lib_name}  SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    GBX_ADD_HEADERS( orcalocalnav ${hdrs} )
    
ENDIF ( build )
