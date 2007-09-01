FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaLaser2dUtil SHARED ${SRCS} )
# special dependency on the generated files (using vehicledesription.h)
ADD_DEPENDENCIES( OrcaLaser2dUtil OrcaInterfaces )

INSTALL_TARGETS( /lib OrcaLaser2dUtil )
INSTALL_FILES( /include/laser2dutil .*\\.h$ )
