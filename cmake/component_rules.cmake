#
# Include and link against required stuff
#
INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/src/libs
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${ICE_HOME}/include
)

# Ice libraries installed in lib64 on 64-bit systems.  Is there a way to tell automatically?
LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/orca
    ${ICE_HOME}/lib
    ${ICE_HOME}/lib64
)

#
# Platform-specific compiler and linker flags
#
IF ( NOT OS_WIN )

    ADD_DEFINITIONS( "-Wall" )
    LINK_LIBRARIES( Ice IceUtil IceStorm ${PROJECT_INTERFACE_LIB} )
    FILE( GLOB SRCS *.cpp )
    FOREACH( SLICE_GENERATED_HEADER ${SLICE_GENERATED_HEADER_FILES} )
      SET_SOURCE_FILES_PROPERTIES( ${SRCS} PROPERTIES OBJECT_DEPENDS ${SLICE_GENERATED_HEADER_FILES} )
    ENDFOREACH( SLICE_GENERATED_HEADER )

ELSE ( NOT OS_WIN )

    ADD_DEFINITIONS( "-Wall -D_CRT_SECURE_NO_DEPRECATE" )
    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice optimized IceUtil optimized IceStorm 
                    debug Iced debug IceUtild debug IceStormd 
                    ${PROJECT_INTERFACE_LIB} )
    FILE( GLOB SRCS *.cpp )
    FOREACH( SLICE_GENERATED_HEADER ${SLICE_GENERATED_HEADER_FILES} )
      SET_SOURCE_FILES_PROPERTIES( ${SRCS} PROPERTIES OBJECT_DEPENDS ${SLICE_GENERATED_HEADER_FILES} )
    ENDFOREACH( SLICE_GENERATED_HEADER )

ENDIF ( NOT OS_WIN )


#
# Project-specific rules
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_rules.cmake )
