INCLUDE_DIRECTORIES( 
    ${ICE_HOME}/include
)

# Ice libraries installed in lib64 on 64-bit systems.  Is there a way to tell automatically?
LINK_DIRECTORIES(
    ${ICE_HOME}/lib
    ${ICE_HOME}/lib64
)

#
# Platform-specific compiler and linker flags
#
IF ( NOT OS_WIN )
    LINK_LIBRARIES( Ice IceUtil IceStorm )
ELSE ( NOT OS_WIN )
    # windows... have to link to different libs depending on build type
    LINK_LIBRARIES( optimized Ice optimized IceUtil optimized IceStorm 
                    debug Iced debug IceUtild debug IceStormd )
ENDIF ( NOT OS_WIN )