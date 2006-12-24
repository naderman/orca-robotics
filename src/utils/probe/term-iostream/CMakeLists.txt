# Glob all source files
FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaProbeTermIostream SHARED ${SRCS} )

INSTALL_TARGETS( /lib OrcaProbeTermIostream )
