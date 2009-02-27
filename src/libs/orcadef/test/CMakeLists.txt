LINK_LIBRARIES( OrcaDef )

ADD_EXECUTABLE( deftest deftest.cpp )
# note that the input file is in the source directory
GBX_ADD_TEST( OrcaDef_Test deftest ${PROJECT_SOURCE_DIR}/src/libs/orcadef/test/def_test_input.def )
