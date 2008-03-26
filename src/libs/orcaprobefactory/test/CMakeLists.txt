INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaProbeFactory )

ADD_EXECUTABLE( factoryinstantiate factoryinstantiate.cpp )
GBX_ADD_TEST( OrcaProbeFactoryInstantiate factoryinstantiate )
