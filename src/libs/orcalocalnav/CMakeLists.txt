SET ( LIB_NAME OrcaLocalNav )
GLOBAL_ADD_LICENSE( LGPL )

SET ( BUILD TRUE )
REQUIRE_OPTION( BUILD LIB ${LIB_NAME} ON )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} OS_LINUX "only Linux OS is supported" )

SET( DEP_LIBS HydroNavUtil )
REQUIRE_VAR( BUILD LIB ${LIB_NAME} HYDRONAVUTIL_INSTALLED "libHydroNavUtil not installed" )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

     INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       ${PROJECT_SOURCE_DIR}/src/components/localnav
       )

    FILE( GLOB SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( ${LIB_NAME}  SHARED ${SRCS} )
    TARGET_LINK_LIBRARIES( ${LIB_NAME} ${DEP_LIBS} )

    INSTALL_FILES( /include/orcalocalnav .*\\.h$ )
    
ENDIF ( BUILD )
