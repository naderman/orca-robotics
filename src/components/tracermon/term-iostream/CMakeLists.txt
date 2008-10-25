INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

FILE( GLOB srcs *.cpp ../termutil/*.cpp )

GBX_ADD_LIBRARY( OrcaTracerMonTermIostream DEFAULT ${srcs} )
