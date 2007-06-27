FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaLaser2dFake SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaLaser2dFake )
