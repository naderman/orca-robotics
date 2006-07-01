INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTeleopJoystick SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaTeleopJoystick )
