IF ( NOT OS_WIN )

OPTION( BUILD_GPS2LOCALISE2D "Try to build Gps2Localise2d" ON )

IF ( BUILD_GPS2LOCALISE2D )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    
    # build common sources into a convenience library
    BUILD_COMPONENT_SOURCES( Gps2Localise2dSources )

    SET( DEP_LIBS
      OrcaIfaceImpl
      OrcaGpsUtil
      OrcaNavUtil
      Gps2Localise2dSources
      )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( gps2localise2d main )
    TARGET_LINK_LIBRARIES( gps2localise2d ${DEP_LIBS} )
    
    # build IceBox service
    GLOBAL_ADD_LIBRARY( OrcaGps2Localise2d SHARED service )
    TARGET_LINK_LIBRARIES( OrcaGps2Localise2d ${DEP_LIBS} )

    GENERATE_CONFIG_FILE( gps2localise2d.def )

ENDIF ( BUILD_GPS2LOCALISE2D )
ENDIF ( NOT OS_WIN )
