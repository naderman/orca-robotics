SET ( lib_name OrcaLocalNav )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )
REQUIRE_VAR( build LIB ${lib_name} ORCA_OS_LINUX "only Linux OS is supported" )

SET( dep_libs HydroNavUtil OrcaObjects )
REQUIRE_VAR( build LIB ${lib_name} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

     INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       ${PROJECT_SOURCE_DIR}/src/components/localnav
       )

    FILE( GLOB srcs *.cpp )

    GLOBAL_ADD_LIBRARY( ${lib_name}  SHARED ${srcs} )
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcalocalnav .*\\.h$ )
    
ENDIF ( build )
