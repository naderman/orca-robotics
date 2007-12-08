SET ( COMP_NAME localnav )
SET ( SERVICE_NAME OrcaLocalNavService )
SET ( OPTION_NAME BUILD_LOCALNAV )

SET ( BUILD TRUE )

IF ( BUILD AND NOT OS_LINUX )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "only Linux OS is supported" )
ENDIF ( BUILD AND NOT OS_LINUX )

OPTION( ${OPTION_NAME} "Try to build ${COMP_NAME}" ON )
IF ( BUILD AND NOT ${OPTION_NAME}  )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT ${OPTION_NAME} )

IF ( BUILD AND NOT HYDRODLL_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "libHydroDll not installed" )
ENDIF ( BUILD AND NOT HYDRODLL_INSTALLED )

IF ( BUILD AND NOT HYDRONAVUTIL_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "libHydroNavUtil not installed" )
ENDIF ( BUILD AND NOT HYDRONAVUTIL_INSTALLED )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
       ${PROJECT_SOURCE_DIR}/src/components/${COMP_NAME}
       ${PROJECT_SOURCE_DIR}/src/libs/orcalocalnav
       )

    ADD_SUBDIRECTORY( testsim )
    ADD_SUBDIRECTORY( localnavutil )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaLocalNav OrcaLocalNavUtil OrcaLocalNavTestSim HydroDll HydroNavUtil )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        # Naming clashes with OrcaLocalNav library in src/libs
        GLOBAL_ADD_LIBRARY( ${SERVICE_NAME} SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( ${SERVICE_NAME} ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

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
      GLOBAL_ADD_TEST( LocalNavVfhTest ${LD_LINUX} --library-path vfhdriver:$ENV{LD_LIBRARY_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${COMP_NAME} --Orca.Config=${PROJECT_BINARY_DIR}/src/components/${COMP_NAME}/${COMP_NAME}.cfg --LocalNav.Config.TestInSimulationMode=1 --LocalNav.Config.Vfh.MaxSpeed=2.0 --LocalNav.Config.Test.NumWaypoints=20 --Orca.RequireRegistry=0 --LocalNav.Config.Test.BatchMode=1 )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD )
