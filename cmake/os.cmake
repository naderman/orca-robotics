#
# Check the OS type.
# Sets the 'OS_INCLUDES' variable, for system-wide Includes that must be set.
#

STRING( REGEX MATCH QNX OS_IS_QNX ${CMAKE_SYSTEM})
STRING( REGEX MATCH Linux OS_IS_LINUX ${CMAKE_SYSTEM})

IF ( OS_IS_QNX )

  MESSAGE("Operating System: QNX")
  SET( OS_INCLUDE_FLAGS "-I/usr/local/include" )
  SET( OS_INCLUDE_DIRECTORIES /usr/local/include )
  SET( OS_LINK_DIRECTORIES    /usr/local/lib /opt/lib )
  SET( OS_LINK_LIBRARIES      iconv m )
  ADD_DEFINITIONS( -shared )
#  SET( OS_DEFINITIONS         -shared )

ENDIF ( OS_IS_QNX )

IF ( OS_IS_LINUX )

  MESSAGE("Operating System: Linux")
  # gcc-4.1 adds stack protection, which makes code robust to buffer-overrun attacks
  #      (see: http://www.trl.ibm.com/projects/security/ssp/)
  # However for some reason this can result in the symbol '__stack_chk_fail_local' not being found.
  # So turn it off.
  ADD_DEFINITIONS( -fno-stack-protector )

ENDIF ( OS_IS_LINUX )
