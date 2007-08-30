IF ( NOT OS_WIN )

OPTION( BUILD_PIXMAPLOADER "Try to build PixMapLoader component" ON )

SET( REALLY_BUILD_PIXMAPLOADER ${BUILD_PIXMAPLOADER} )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "Not building PixMapLoader: Qt4 not found." )
  SET( REALLY_BUILD_PIXMAPLOADER FALSE )
ENDIF ( NOT QT4_FOUND )

IF ( REALLY_BUILD_PIXMAPLOADER )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaMapLoad OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pixmaploader ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( pixmaploader ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaPixMapLoader SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaPixMapLoader ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( pixmaploader.def )

ENDIF ( REALLY_BUILD_PIXMAPLOADER )
ENDIF ( NOT OS_WIN  )
