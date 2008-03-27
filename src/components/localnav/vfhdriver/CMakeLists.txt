INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

FILE( GLOB srcs *.cpp *.cc )

GBX_ADD_LIBRARY( OrcaLocalNavVfh SHARED ${srcs} )
