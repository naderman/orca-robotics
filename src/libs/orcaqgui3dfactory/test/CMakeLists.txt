include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

link_libraries( OrcaQGui3dFactory ${dep_libs} )

add_executable( default3dfactoryinstantiate defaultfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaQGui3dDefaultFactoryInstantiate default3dfactoryinstantiate )
