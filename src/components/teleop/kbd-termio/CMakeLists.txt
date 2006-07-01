INCLUDE( ${PROJECT_SOURCE_DIR}/config/include_orcaice.cmake )

# Source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaTeleopKeyboardTermio SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaTeleopKeyboardTermio )
