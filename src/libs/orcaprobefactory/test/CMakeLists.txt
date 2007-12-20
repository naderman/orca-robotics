INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaProbeFactory )

ADD_EXECUTABLE( factoryinstantiate factoryinstantiate.cpp )
GLOBAL_ADD_TEST( OrcaProbeFactoryInstantiate factoryinstantiate )
