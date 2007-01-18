IF ( OS_LINUX )

OPTION( BUILD_LOCALNAV "Try to build LocalNav component" ON )

IF ( BUILD_LOCALNAV )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

    ADD_SUBDIRECTORY( vfhdriver )
    MESSAGE( STATUS "    VfhDriver for LocalNav - can be built" )

    INCLUDE_DIRECTORIES(
      ${PROJECT_SOURCE_DIR}/src/components/localnav
      )

    ADD_SUBDIRECTORY( testsim )
    ADD_SUBDIRECTORY( localnavutil )

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( LocalNavSources )
    
    SET( DEP_LIBS
      LocalNavSources
      # OrcaLocalNavVfh
      OrcaLocalNavUtil
      OrcaDynamicLoad
      OrcaLocalNavTestSim
      OrcaNavUtil )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( localnav main )
    TARGET_LINK_LIBRARIES( localnav ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLocalNav SHARED service )
    TARGET_LINK_LIBRARIES( OrcaLocalNav ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( localnav.def )

    IF ( BUILD_TESTS )
      #
      # AlexB: The /lib/ld-linux stuff is a bit of magic to make Linux
      # find the vfh library so it can be dynamically loaded.
      #
      ADD_TEST( LocalNavVfhTest /lib/ld-linux.so.2 --library-path vfhdriver:$ENV{LD_LIBRARY_PATH} localnav --Orca.Config=${PROJECT_BINARY_DIR}/src/components/localnav/localnav.cfg --LocalNav.Config.TestMode=1 --LocalNav.Config.Vfh.MaxSpeed=2.0 --LocalNav.Config.Test.BatchMode=1 )
    ENDIF ( BUILD_TESTS )

ENDIF ( BUILD_LOCALNAV )
ENDIF ( OS_LINUX )
