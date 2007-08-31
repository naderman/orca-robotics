INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

FILE( GLOB SRCS *.cpp ../termutil/*.cpp )

GLOBAL_ADD_LIBRARY( OrcaTracerMonTermIostream SHARED ${SRCS} )
