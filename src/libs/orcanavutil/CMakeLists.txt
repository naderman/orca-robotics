SET ( LIB_NAME OrcaNavUtil )

REQUIRE_OPTION( LIB ${LIB_NAME} ON )
REQUIRE_VAR( LIB ${LIB_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
     
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} HydroNavUtil )
    INSTALL_FILES( /include/orcanavutil .*\\.h$ ) 

ENDIF ( BUILD )
