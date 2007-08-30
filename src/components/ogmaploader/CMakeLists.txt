IF ( NOT OS_WIN )

OPTION( BUILD_OGMAPLOADER "Try to build OgMapLoader component" ON )

SET( REALLY_BUILD_OGMAPLOADER ${BUILD_OGMAPLOADER} )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "Not building OgMapLoader: Qt4 not found." )
  SET( REALLY_BUILD_OGMAPLOADER FALSE )
ENDIF ( NOT QT4_FOUND )

IF ( REALLY_BUILD_OGMAPLOADER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl OrcaMapLoad )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ogmaploader ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ogmaploader ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaOgMapLoader SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaOgMapLoader ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ogmaploader.def )

ENDIF ( REALLY_BUILD_OGMAPLOADER )
ENDIF ( NOT OS_WIN  )
