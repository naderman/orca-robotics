#
# Include and link against required stuff
#
INCLUDE_DIRECTORIES( 
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${ICE_HOME}/include
)

LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/orca
    ${ICE_HOME}/lib
)

#
# Platform-specific compiler and linker flags
#
IF ( NOT WIN32 )

    ADD_DEFINITIONS( "-Wall" )
    LINK_LIBRARIES( Ice IceUtil IceStorm ${PROJECT_INTERFACE_LIB} )

ELSE ( NOT WIN32 )

    ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice optimized IceUtil optimized IceStorm 
                    debug Iced debug IceUtild debug IceStormd 
                    ${PROJECT_INTERFACE_LIB} )

ENDIF ( NOT WIN32 )


#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_rules.cmake )
