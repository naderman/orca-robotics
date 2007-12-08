OPTION( BUILD_FAITHLOCALISER "Try to build FaithLocaliser component" ON )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( faithlocaliser "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_FAITHLOCALISER )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( faithlocaliser "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_FAITHLOCALISER )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

    FIND_COMPONENT_SOURCES( COMP_SRCS )
    SET( COMP_DEPS OrcaIfaceImpl )

    # build stand-alone component  
    GLOBAL_ADD_EXECUTABLE( faithlocaliser ${COMP_SRCS} main )
    TARGET_LINK_LIBRARIES( faithlocaliser ${COMP_DEPS} )
    
    # build IceBox service
    IF ( BUILD_SERVICES )
        GLOBAL_ADD_LIBRARY( OrcaFaithLocaliser SHARED ${COMP_SRCS} service )
        TARGET_LINK_LIBRARIES( OrcaFaithLocaliser ${COMP_DEPS} )    
    ENDIF ( BUILD_SERVICES )

    GENERATE_CONFIG_FILE( faithlocaliser.def )

ENDIF ( BUILD )
