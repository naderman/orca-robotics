OPTION( BUILD_PIXMAPLOADER "Try to build PixMapLoader component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pixmaploader "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_PIXMAPLOADER )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pixmaploader "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_PIXMAPLOADER )

IF ( BUILD AND NOT QT4_FOUND )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( pixmaploader "Qt4 not found" )
ENDIF ( BUILD AND NOT QT4_FOUND )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS HydroMapLoad OrcaIfaceImpl )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( pixmaploader ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( pixmaploader ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaPixMapLoader SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaPixMapLoader ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( pixmaploader.def )

ENDIF ( BUILD )
