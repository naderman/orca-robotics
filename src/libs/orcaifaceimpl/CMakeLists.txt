SET ( lib_name OrcaIfaceImpl )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    IF ( NOT ORCA_OS_WIN )
        GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE ( NOT ORCA_OS_WIN )
        GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF ( NOT ORCA_OS_WIN )

    TARGET_LINK_LIBRARIES( ${lib_name} OrcaObjects )
    
    INSTALL_FILES( /include/orcaifaceimpl .*\\.h$ )
    
    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )