FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( OrcaIceDetail STATIC ${SRCS} )
# special dependency on the generated files
ADD_DEPENDENCIES( OrcaIceDetail OrcaInterfaces )
