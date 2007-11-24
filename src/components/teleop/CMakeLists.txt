IF ( NOT OS_WIN )

OPTION( BUILD_TELEOP "Try to build Teleop" ON )

IF ( BUILD_TELEOP )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    
    # This gathers all the .cpp files into the variable 'SRCS'
    FILE( GLOB COMP_SRCS *.cpp )
    SET( COMP_DEPS HydroDll HydroInterfaces )

    GLOBAL_ADD_EXECUTABLE( teleop ${COMP_SRCS} )
    TARGET_LINK_LIBRARIES( teleop ${COMP_DEPS} )
    GENERATE_CONFIG_FILE( teleop.def )

ENDIF ( BUILD_TELEOP )
ENDIF ( NOT OS_WIN )
