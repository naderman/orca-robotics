INCLUDE( ${ORCA_CMAKE_DIR}/include_orcaice.cmake )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTeleopKeyboardNcurses SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaTeleopKeyboardNcurses )
