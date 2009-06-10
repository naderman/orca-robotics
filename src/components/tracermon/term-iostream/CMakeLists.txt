include( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )

file( GLOB srcs *.cpp ../termutil/*.cpp )

set( lib_version ${GBX_PROJECT_VERSION} )
GBX_ADD_LIBRARY( OrcaTracerMonTermIostream DEFAULT ${lib_version} ${srcs} )
