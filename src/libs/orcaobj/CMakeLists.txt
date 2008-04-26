ORCA_SET_LIBRARY_NAME( OrcaObjects )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
# This is one of fundamental libs. It is not optional (but maybe it should change)

SET( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
GBX_REQUIRE_INSTALLS( build LIB ${LIB_NAME} ${ext_libs} )

IF( build )

    # Do not include UseComponentRules here because it will link to OrcaIce
    # creating circular dependency.
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIce.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydroUtil.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceString.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    # for some reason cannot get shared libs working in windows
    IF(NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${LIB_NAME} SHARED ${srcs} )
    ELSE (NOT GBX_OS_WIN)
        GBX_ADD_LIBRARY( ${LIB_NAME} STATIC ${srcs} )
    ENDIF (NOT GBX_OS_WIN)
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} OrcaInterfaces OrcaIce )
    
    # special dependency on the generated files
    ADD_DEPENDENCIES( ${LIB_NAME} OrcaInterfaces )
    
    GBX_ADD_HEADERS( orcaobj ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
