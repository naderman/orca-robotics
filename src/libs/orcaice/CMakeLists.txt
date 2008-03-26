SET ( lib_name OrcaIce )
GBX_ADD_LICENSE( LGPL )

SET ( build TRUE )
# this library is not optional

SET ( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

IF ( build )

    # these are the same as in UseComponentRules but without project rules
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydroIceUtil.cmake )
    
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/detail )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp detail/*.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} )
    
    IF( NOT GBX_OS_WIN )
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE ( NOT GBX_OS_WIN )
        # building shared libs is complicated in windows
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF ( NOT GBX_OS_WIN )
    
    TARGET_LINK_LIBRARIES( ${lib_name} ${dep_libs} )

    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    GBX_ADD_HEADERS( orcaice ${hdrs} )
    
    IF ( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY ( test )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )
