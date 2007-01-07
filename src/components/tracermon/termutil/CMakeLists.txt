ADD_DEFINITIONS( -Wall )

FILE( GLOB SRCS *.cpp )

ADD_LIBRARY( termutil STATIC ${SRCS} )

ADD_DEPENDENCIES( termutil OrcaInterfaces )
