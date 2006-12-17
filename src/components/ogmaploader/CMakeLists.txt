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

    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
      )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( OgMapLoaderSources )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ogmaploader main )
    TARGET_LINK_LIBRARIES( ogmaploader OgMapLoaderSources OrcaOgMapLoad ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaOgMapLoader SHARED service )
    TARGET_LINK_LIBRARIES( OrcaOgMapLoader OgMapLoaderSources OrcaOgMapLoad ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    GENERATE_CONFIG_FILE( ogmaploader.def )

ENDIF ( REALLY_BUILD_OGMAPLOADER )
ENDIF ( NOT OS_WIN  )
