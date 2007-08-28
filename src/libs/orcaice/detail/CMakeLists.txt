FILE( GLOB SRCS *.cpp )

IF( NOT OS_WIN )
  # This is needed because this STATIC library is gonna
  # get built into libOrcaIce, which is a SHARED library.
  ADD_DEFINITIONS( -fPIC )
ENDIF ( NOT OS_WIN )

ADD_LIBRARY( OrcaIceDetail STATIC ${SRCS} )
# special dependency on the generated files
ADD_DEPENDENCIES( OrcaIceDetail OrcaInterfaces )
