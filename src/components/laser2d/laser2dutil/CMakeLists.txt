FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaLaser2dUtil ${SRCS} )

INSTALL_TARGETS( /lib OrcaLaser2dUtil )
INSTALL_FILES( /include/laser2dutil .*\\.h$ )
