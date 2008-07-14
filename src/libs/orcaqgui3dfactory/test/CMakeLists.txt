INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaQGui3dFactory ${dep_libs} )

ADD_EXECUTABLE( defaultfactoryinstantiate defaultfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaQGui3dDefaultFactoryInstantiate defaultfactoryinstantiate )
