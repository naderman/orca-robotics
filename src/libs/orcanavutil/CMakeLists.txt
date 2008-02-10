SET ( lib_name OrcaNavUtil )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( dep_libs HydroNavUtil OrcaObjects )
REQUIRE_VAR( build LIB ${lib_name} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
     
    GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    INSTALL_FILES( /include/orcanavutil .*\\.h$ ) 

ENDIF ( build )
