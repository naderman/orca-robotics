include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

link_libraries( OrcaQGui2dFactory ${dep_libs} )

add_executable( defaultfactory2dinstantiate defaultfactoryinstantiate.cpp )
GBX_ADD_TEST( OrcaQGui2dFactory_Instantiate defaultfactory2dinstantiate )
