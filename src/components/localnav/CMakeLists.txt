ORCA_SET_COMPONENT_NAME( LocalNav )

SET( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

SET( ext_libs HydroDll HydroNavUtil HydroGeom2d )
GBX_REQUIRE_INSTALLS( build EXE ${APP_NAME} ${ext_libs} )

SET( int_libs OrcaLocalNav OrcaNavUtil OrcaIfaceImpl OrcaOgMap OrcaPathPlan OrcaSim2d )
GBX_REQUIRE_TARGETS( build EXE ${APP_NAME} ${int_libs} )

IF( build )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/components/${APP_NAME}
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       )

    ADD_SUBDIRECTORY( testsim )

    FILE( GLOB srcs *.cpp )
    SET( dep_libs ${ext_libs} ${int_libs} OrcaLocalNavTestSim )

    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${APP_NAME} ${APP_CTRLC_HANDLER} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
    TARGET_LINK_LIBRARIES( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    IF( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${SERVICE_NAME} ${COMP_NAMESPACE} ${COMP_NAME} ${srcs} )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${dep_libs} ) 
    ENDIF( ORCA_BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

    IF( ORCA_BUILD_TESTS )

      # Check for 32-bit vs 64-bit
      IF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET( IS_32_BIT FALSE )
        SET( IS_64_BIT TRUE )
      ELSE ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        SET( IS_32_BIT TRUE )
        SET( IS_64_BIT FALSE )
      ENDIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )

      # Work out what ld-linux executable to use
      IF( IS_64_BIT )
        SET( LD_LINUX /lib/ld-linux-x86-64.so.2 )
      ELSE ( IS_64_BIT )
        SET( LD_LINUX /lib/ld-linux.so.2 )
      ENDIF( IS_64_BIT )

      #
      # AlexB: The /lib/ld-linux stuff is a bit of magic to make Linux
      # find the vfh library so it can be dynamically loaded.
      #
      GBX_ADD_TEST( LocalNav_VfhTest ${LD_LINUX}
        --library-path ${CMAKE_CURRENT_BINARY_DIR}/../../libs/orcavfh:$ENV{LD_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}
        --Orca.Config=${CMAKE_CURRENT_BINARY_DIR}/${APP_NAME}.cfg
        --LocalNav.Config.TestInSimulationMode=1
        --LocalNav.Config.Vfh.MaxSpeed=2.0
        --LocalNav.Config.Test.NumWaypoints=20
        --Orca.RequireRegistry=0
        --LocalNav.Config.Test.BatchMode=1 )
    ENDIF( ORCA_BUILD_TESTS )

ENDIF( build )
