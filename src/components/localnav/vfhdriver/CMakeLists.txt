ADD_DEFINITIONS( -Wall )

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

FILE( GLOB srcs *.cpp *.cc )

ADD_LIBRARY( OrcaLocalNavVfh SHARED ${srcs} )
INSTALL_TARGETS( /lib OrcaLocalNavVfh )
