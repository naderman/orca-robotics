OPTION( BUILD_OGMAPLOADER "Try to build OgMapLoader component" ON )

SET( REALLY_BUILD_OGMAPLOADER ${BUILD_OGMAPLOADER} )

IF ( NOT QT4_FOUND )
  MESSAGE( STATUS "Not building OgMapLoader: Qt4 not found." )
  SET( REALLY_BUILD_OGMAPLOADER FALSE )
ENDIF ( NOT QT4_FOUND )

IF ( NOT WIN32 AND REALLY_BUILD_OGMAPLOADER )

    INCLUDE( ${PROJECT_SOURCE_DIR}/config/component_rules.cmake )
    INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

    INCLUDE_DIRECTORIES(
        ${QT_INCLUDES}
      )
    
    # Not sure how to find "all .cpp files except main.cpp", have to do it by hand
    SET( COMMON_SRCS component ogmapI maploader fakemaploader )
    
    # Files for the stand-alone application
    SET( COMP_SRCS main ${COMMON_SRCS} )
    
    # Files for the icebox service
    SET( SERVICE_SRCS serviceI ${COMMON_SRCS} )

    # build a stand-alone component
    GLOBAL_ADD_EXECUTABLE( ogmaploader ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ogmaploader OrcaOgMapLoad ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )

    # build an IceBox service
    ADD_LIBRARY( OrcaOgMapLoader SHARED ${SERVICE_SRCS} )
    
    # important to let linker know about this inter-library dependency
    # otherwise IceBox fails to load the service
    TARGET_LINK_LIBRARIES( OrcaOgMapLoader ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} )
    INSTALL_TARGETS( /lib OrcaOgMapLoader )

    GENERATE_CONFIG_FILE( ogmaploader.def )

ENDIF ( NOT WIN32 AND REALLY_BUILD_OGMAPLOADER )
