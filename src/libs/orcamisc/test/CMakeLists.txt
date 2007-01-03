INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs/orcamisc )

ADD_EXECUTABLE( rand_test rand.cpp )

TARGET_LINK_LIBRARIES( rand_test OrcaMisc )

## Tests ##
ADD_TEST( OrcaMiscRand           rand_test )
MESSAGE( STATUS "Planning to Build Test     : OrcaMiscRand [libOrcaMisc]")
