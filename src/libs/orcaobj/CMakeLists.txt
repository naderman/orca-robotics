SET( lib_name OrcaObjects )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
# This is one of fundamental libs. It is not optional (but maybe it should change)

SET( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
GBX_REQUIRE_INSTALLS( build LIB ${lib_name} ${ext_libs} )

IF( build )

    # Do not include UseComponentRules here because it will link to OrcaIce
    # creating circular dependency.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydroUtil.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    # for some reason cannot get shared libs working in windows
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${lib_name} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${lib_name} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${lib_name} OrcaInterfaces OrcaIce )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${lib_name} OrcaInterfaces )
    
    GBX_ADD_HEADERS( orcaobj ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
