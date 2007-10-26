INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaQGui2dFactory )

ADD_EXECUTABLE( defaultfactoryinstantiate defaultfactoryinstantiate.cpp )
GLOBAL_ADD_TEST( OrcaQGui2dDefaultFactoryInstantiate defaultfactoryinstantiate )
