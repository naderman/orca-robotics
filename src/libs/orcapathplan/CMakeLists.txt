ORCA_SET_LIBRARY_NAME( OrcaPathPlan )
GBX_ADD_LICENSE( LGPL )

SET( build TRUE )
GBX_REQUIRE_OPTION( build LIB ${LIB_NAME} ON )
GBX_REQUIRE_VAR( build LIB ${LIB_NAME} GBX_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroPathPlan )
GBX_REQUIRE_INSTALLS( build LIB ${LIB_NAME} ${ext_libs} )

SET( int_libs OrcaObjects )
GBX_REQUIRE_TARGETS( build LIB ${LIB_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FILE( GLOB hdrs *.h )
    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${int_libs} ${ext_libs} )

    GBX_ADD_LIBRARY( ${LIB_NAME} DEFAULT ${srcs} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${dep_libs} )

    GBX_ADD_HEADERS( ${LIB_INSTALL} ${hdrs} )
    
ENDIF( build )
