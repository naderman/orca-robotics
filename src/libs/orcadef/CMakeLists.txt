SET ( lib_name OrcaDef )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

SET( dep_libs OrcaObjects )
REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    IF(NOT ORCA_OS_WIN)
        GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT ORCA_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT ORCA_OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaObjects )
    
    INSTALL_FILES( /include/orcadef .*\\.h$ )
    
    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )
