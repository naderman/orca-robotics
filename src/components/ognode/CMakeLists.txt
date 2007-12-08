SET ( COMP_NAME ognode )
SET ( SERVICE_NAME OrcaFaithLocaliser )
SET ( OPTION_NAME BUILD_OGNODE )

SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

OPTION( ${OPTION_NAME} "Try to build ${COMP_NAME}" ON )
IF ( BUILD AND NOT ${OPTION_NAME}  )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_LIBRARY( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT ${OPTION_NAME} )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    #
    # This gathers all the source files into the variable 'SRCS'
    #
    FILE( GLOB SRCS *.cpp )           
    
    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} OrcaOgFusion OrcaOgMap OrcaIfaceImpl )
    
    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
