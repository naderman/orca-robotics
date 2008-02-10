# Glob all source files
FILE( GLOB srcs *.cpp )

ADD_LIBRARY( OrcaProbeTermIostream SHARED ${srcs} )

INSTALL_TARGETS( /lib OrcaProbeTermIostream )
