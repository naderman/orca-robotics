SET ( LIB_NAME OrcaNavUtil )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )

SET( DEP_LIBS HydroNavUtil )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    FILE( GLOB SRCS *.cpp )
     
    GLOBAL_ADD_LIBRARY( ${LIB_NAME} SHARED ${SRCS} )
    
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEP_LIBS} )

    INSTALL_FILES( /include/orcanavutil .*\\.h$ ) 

ENDIF ( BUILD )
