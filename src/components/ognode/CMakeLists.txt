SET ( COMP_NAME ognode )
SET ( SERVICE_NAME OrcaFaithLocaliser )

REQUIRE_OPTION( EXE ${COMP_NAME} ON )
SET ( NOT_OS_WIN NOT OS_WIN )
REQUIRE_VAR( EXE ${COMP_NAME} NOT_OS_WIN "Windows OS is not  supported" )

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
