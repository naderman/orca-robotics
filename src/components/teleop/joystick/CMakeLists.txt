INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTeleopJoystick SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaTeleopJoystick )
