OPTION( BUILD_OGMAPLOADER "Try to build OgMapLoader component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ogmaploader "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_OGMAPLOADER )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ogmaploader "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_OGMAPLOADER )

IF ( BUILD AND NOT QT4_FOUND )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ogmaploader "Qt4 not found" )
ENDIF ( BUILD AND NOT QT4_FOUND )

IF ( BUILD AND NOT HYDROMAPLOAD_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ogmaploader "libHydroMapLoad not installed" )
ENDIF ( BUILD AND NOT HYDROMAPLOAD_INSTALLED )

IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )
    
    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl HydroMapLoad )
    
    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( ogmaploader ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( ogmaploader ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaOgMapLoader SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaOgMapLoader ${COMP_DEPS} ) 
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( ogmaploader.def )

ENDIF ( BUILD )
