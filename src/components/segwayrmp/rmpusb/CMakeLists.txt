INCLUDE( ${PROJ_SOURCE_DIR}/config/component_rules.cmake )

# This globbing option does not work for some reason.
#FILE( GLOB SRCS {*.cc,*.cpp} )
SET( SRCS rmpusbdriver canio_usb_ftdi )

INCLUDE_DIRECTORIES( 
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define some stuff to fool Player code
ADD_DEFINITIONS( -DHAVE_STDINT_H )

ADD_LIBRARY( rmp ${SRCS} )
