OPTION( BUILD_TELEOP "Try to build Teleop" ON )

SET ( COMP_NAME teleop )
SET ( SERVICE_NAME OrcaFaithLocaliser )
SET ( BUILD TRUE )

IF ( BUILD AND OS_WIN )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "Windows OS is not supported" )
ENDIF ( BUILD AND OS_WIN )

IF ( BUILD AND NOT BUILD_TELEOP )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "disabled by user, use ccmake to enable" )
ENDIF ( BUILD AND NOT BUILD_TELEOP )

IF ( BUILD AND NOT HYDRODLL_INSTALLED )
    SET ( BUILD FALSE )
    GLOBAL_NOT_ADD_EXECUTABLE( ${COMP_NAME} "libHydroDll not installed" )
ENDIF ( BUILD AND NOT HYDRODLL_INSTALLED )


IF ( BUILD )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS HydroDll HydroInterfaces )

    GLOBAL_ADD_EXECUTABLE( ${COMP_NAME} ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( ${COMP_NAME} ${COMP_DEPS} )

    GENERATE_CONFIG_FILE( ${COMP_NAME}.def )

ENDIF ( BUILD )
