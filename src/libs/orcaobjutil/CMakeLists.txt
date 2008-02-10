SET ( lib_name OrcaObjectUtil )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_OPTION( build LIB ${lib_name} ON )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB srcs *.cpp )
    
    IF(NOT ORCA_OS_WIN)
        GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT ORCA_OS_WIN)
        # for some reason cannot get shared libs working in windows
        GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT ORCA_OS_WIN)
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaObjects )
    
    INSTALL_FILES( /include/orcaobjutil .*\\.h$ )

ENDIF ( build )
