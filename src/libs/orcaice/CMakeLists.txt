ORCA_SET_LIBRARY_NAME( OrcaIce )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
# This is one of fundamental libs. It is not optional.

SET( ext_libs GbxUtilAcfr GbxIceUtilAcfr HydroUtil )
GBX_REQUIRE_INSTALLS( build LIB ${LIB_NAME} ${ext_libs} )

IF( build )

    # these are the same as in UseComponentRules but without project rules
    INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseLibProjectInterfaces.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseIceBasics.cmake )
    INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydroIceUtil.cmake )
    # debug only!
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIfaceString.cmake )
    
    # files from detail dir refer to public files
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
       ${CMAKE_CURRENT_SOURCE_DIR}/detail )
    
    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp detail/*.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} )
    
    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} )
    
    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
    IF( ORCA_BUILD_TESTS )
        ADD_SUBDIRECTORY( test )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
