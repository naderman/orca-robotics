IF ( OS_LINUX )

OPTION( BUILD_LOCALNAV "Try to build LocalNav component" ON )

IF ( BUILD_LOCALNAV )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/components/localnav
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       )

    ADD_SUBDIRECTORY( testsim )
    ADD_SUBDIRECTORY( localnavutil )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaLocalNav OrcaLocalNavUtil HydroDll OrcaLocalNavTestSim OrcaNavUtil )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( localnav ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( localnav ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        # Naming clashes with OrcaLocalNav library in src/libs
        GLOBAL_ADD_LIBRARY( OrcaLocalNavComponent SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaLocalNavComponent ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( localnav.def )

    IF ( BUILD_TESTS )

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
      ADD_TEST( LocalNavVfhTest ${LD_LINUX} --library-path vfhdriver:$ENV{LD_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/localnav --Orca.Config=${PROJECT_BINARY_DIR}/src/components/localnav/localnav.cfg --LocalNav.Config.TestInSimulationMode=1 --LocalNav.Config.Vfh.MaxSpeed=2.0 --LocalNav.Config.Test.NumWaypoints=20 --Orca.RequireRegistry=0 --LocalNav.Config.Test.BatchMode=1 )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD_LOCALNAV )
ENDIF ( OS_LINUX )
