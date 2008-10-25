ORCA_SET_LIBRARY_NAME( OrcaObjects )
SET( LIB_INSTALL orcaobj )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
# This is one of fundamental libs. It is not optional (but maybe it should change)

# these 2 lines just check prereq's, but linking is specified in the macros below.
SET( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroIceUtil )
GBX_REQUIRE_INSTALLS( build LIB ${LIB_NAME} ${ext_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectIfaceString.cmake )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
