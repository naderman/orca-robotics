INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/orcamisc )

LINK_LIBRARIES( OrcaMisc )

ADD_EXECUTABLE( rand_test rand.cpp )
GLOBAL_ADD_TEST( OrcaMiscRand rand_test )
