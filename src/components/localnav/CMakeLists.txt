ORCA_SET_COMPONENT_NAME( LocalNav )

set( build TRUE )
GBX_REQUIRE_OPTION( build EXE ${APP_NAME} ON )
GBX_REQUIRE_VAR( build EXE ${APP_NAME} GBX_OS_LINUX "only Linux OS is supported" )

set( dep_libs HydroDll HydroNavUtil HydroGeom2d OrcaLocalNav OrcaLocalNavTest OrcaNavUtil OrcaIfaceImpl OrcaOgMap OrcaPathPlan )
GBX_REQUIRE_LIBS( build EXE ${APP_NAME} ${dep_libs} )

if( build )

    include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    include_directories(
       ${PROJECT_SOURCE_DIR}/src/components/${APP_NAME}
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       )

    file( GLOB srcs *.cpp )

    # build stand-alone component  
    ORCA_WRAP_MAINTHREAD_INTO_APP( ${srcs} )
    target_link_libraries( ${APP_NAME} ${dep_libs} )
    
    # build IceBox service
    if( ORCA_BUILD_SERVICES )
        ORCA_WRAP_MAINTHREAD_INTO_SERVICE( ${srcs} )
        target_link_libraries( ${SERVICE_NAME} ${dep_libs} ) 
    endif( ORCA_BUILD_SERVICES )

    ORCA_GENERATE_CONFIG_FILE( ${APP_NAME}.def )

    if( ORCA_BUILD_TESTS )

      # Check for 32-bit vs 64-bit
      if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set( IS_32_BIT FALSE )
        set( IS_64_BIT TRUE )
      else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
        set( IS_32_BIT TRUE )
        set( IS_64_BIT FALSE )
      endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

      # Work out what ld-linux executable to use
      if( IS_64_BIT )
        set( LD_LINUX /lib/ld-linux-x86-64.so.2 )
      else( IS_64_BIT )
        set( LD_LINUX /lib/ld-linux.so.2 )
      endif( IS_64_BIT )

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
    endif( ORCA_BUILD_TESTS )

endif( build )
