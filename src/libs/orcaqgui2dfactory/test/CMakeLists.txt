INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

LINK_LIBRARIES( OrcaQGui2dFactory ${dep_libs} )

ADD_EXECUTABLE( defaultfactory2dinstantiate defaultfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaQGui2dDefaultFactoryInstantiate defaultfactory2dinstantiate )
