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

    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
      )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( PixMapLoaderSources )

    SET( DEP_LIBS
      PixMapLoaderSources
      OrcaMapLoad 
      ${QT_QTGUI_LIBRARY} 
      ${QT_QTCORE_LIBRARY}
      OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pixmaploader main )
    TARGET_LINK_LIBRARIES( pixmaploader ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaPixMapLoader SHARED service )
    TARGET_LINK_LIBRARIES( OrcaPixMapLoader ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( pixmaploader.def )

ENDIF ( REALLY_BUILD_PIXMAPLOADER )
ENDIF ( NOT OS_WIN  )
