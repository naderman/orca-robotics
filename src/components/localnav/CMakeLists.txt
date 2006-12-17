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

    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( LocalNavSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( localnav main )
    TARGET_LINK_LIBRARIES( localnav LocalNavSources OrcaLocalNavVfh OrcaNavUtil )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaLocalNav SHARED service )
    TARGET_LINK_LIBRARIES( OrcaLocalNav LocalNavSources OrcaLocalNavVfh OrcaNavUtil )

    GENERATE_CONFIG_FILE( localnav.def )

ENDIF ( BUILD_LOCALNAV )
ENDIF ( OS_LINUX )
