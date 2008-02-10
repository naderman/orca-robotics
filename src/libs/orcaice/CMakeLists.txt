SET ( lib_name OrcaIce )
GLOBAL_ADD_LICENSE( LGPL )

SET ( build TRUE )
REQUIRE_VAR( build LIB ${lib_name} HYDROUTIL_INSTALLED "libHydroUtil not found" )

# SET( dep_libs OrcaObjects )
# REQUIRE_TARGETS( build LIB ${lib_name} ${dep_libs} )

IF ( build )

    # these are the same as in UseComponentRules but without project rules
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseHydroIceUtil.cmake )
    
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/detail )
    
    FILE( GLOB srcs *.cpp detail/*.cpp )
    
    IF( NOT ORCA_OS_WIN )
        GLOBAL_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE ( NOT ORCA_OS_WIN )
        # building shared libs is complicated in windows
        GLOBAL_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF ( NOT ORCA_OS_WIN )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    INSTALL_FILES( /include/orcaice .*\\.h$ )
    
    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )
