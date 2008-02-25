SET_COMPONENT_NAMES ( LocalNav )
SET ( APP_CTRLC_HANDLER 0 )

SET ( build TRUE )
REQUIRE_OPTION( build EXE ${APP_NAME} ON )
REQUIRE_VAR( build EXE ${APP_NAME} ORCA_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroDll HydroNavUtil HydroGeom2d )
REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( int_libs OrcaLocalNav OrcaNavUtil OrcaIfaceImpl OrcaOgMap OrcaPathPlan )
REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF ( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/components/${APP_NAME}
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       )

    ADD_SUBDIRECTORY( testsim )
    ADD_SUBDIRECTORY( localnavutil )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} OrcaLocalNavTestSim OrcaLocalNavUtil )

    # build stand-alone component  
    WRAP_COMPONENT_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF ( ORCA_BUILD_SERVICES )
        WRAP_COMPONENT_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} ) 
    ENDIF ( ORCA_BUILD_SERVICES )

    IF ( ORCA_BUILD_TESTS )

      # Check for 32-bit vs 64-bit
      IF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET ( IS_32_BIT FALSE )
        SET ( IS_64_BIT TRUE )
      ELSE ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET ( IS_32_BIT TRUE )
        SET ( IS_64_BIT FALSE )
      ENDIF ( CMAKE_SIZEOF_VOID_P EQUAL 8 )

      # Work out what ld-linux executable to use
      IF ( IS_64_BIT )
        SET ( LD_LINUX /lib/ld-linux-x86-64.so.2 )
      ELSE ( IS_64_BIT )
        SET ( LD_LINUX /lib/ld-linux.so.2 )
      ENDIF ( IS_64_BIT )

      #
      # AlexB: The /lib/ld-linux stuff is a bit of magic to make Linux
      # find the vfh library so it can be dynamically loaded.
      #
      GLOBAL_ADD_TEST( LocalNavVfhTest ${LD_LINUX}
        --library-path vfhdriver:$ENV{LD_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}
        --Orca.Config=${PROJECT_BINARY_DIR}/src/components/${APP_NAME}/${APP_NAME}.cfg
        --LocalNav.Config.TestInSimulationMode=1
        --LocalNav.Config.Vfh.MaxSpeed=2.0
        --LocalNav.Config.Test.NumWaypoints=20
        --Orca.RequireRegistry=0
        --LocalNav.Config.Test.BatchMode=1 )
    ENDIF ( ORCA_BUILD_TESTS )

ENDIF ( build )
