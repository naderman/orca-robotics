include( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )

link_libraries( OrcaProbeFactory )

add_executable( factoryinstantiate factoryinstantiate.cpp )
GBX_ADD_TEST( OrcaProbeFactory_Instantiate factoryinstantiate )
