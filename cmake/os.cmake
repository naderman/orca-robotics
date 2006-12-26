#
# Check the OS type.
# Sets the 'OS_INCLUDES' variable, for system-wide Includes that must be set.
#


# CMake does not distinguish Linux from other Unices.
STRING( REGEX MATCH Linux OS_LINUX ${CMAKE_SYSTEM_NAME})

# Rename CMake's variable to something which makes more sense.
IF ( QNXNTO )
    SET ( OS_QNX TRUE BOOL INTERNAL )
ENDIF ( QNXNTO )

# In windows we just mirror CMake's own variable
IF ( WIN32 )
    SET ( OS_WIN TRUE BOOL INTERNAL )
ENDIF ( WIN32 )

# In MacOS X we just mirror CMake's own variable
IF ( APPLE )
    SET ( OS_MAC TRUE BOOL INTERNAL )
ENDIF ( APPLE )


# From now on, use our own OS flags

IF ( OS_LINUX )
    MESSAGE ( STATUS "Running on Linux" )
ENDIF ( OS_LINUX )

IF ( OS_QNX )
    MESSAGE ( STATUS "Running on QNX" )
#   SET( OS_INCLUDE_FLAGS "-I/usr/local/include" )
#   SET( OS_INCLUDE_DIRECTORIES /usr/local/include )
#   SET( OS_LINK_DIRECTORIES    /usr/local/lib /opt/lib )
#   SET( OS_LINK_LIBRARIES      iconv m )
#   SET( OS_DEFINITIONS         -shared )
    ADD_DEFINITIONS( -shared -fexceptions )
ENDIF ( OS_QNX )


IF ( OS_WIN )
    MESSAGE ( STATUS "Running on Windows" )
ENDIF ( OS_WIN )