include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

file( GLOB srcs *.cpp ../termutil/*.cpp )

GBX_ADD_LIBRARY( OrcaTracerMonTermIostream DEFAULT ${srcs} )
