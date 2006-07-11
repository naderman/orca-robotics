ADD_DEFINITIONS( -Wall )

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

FILE( GLOB SRCS *.cpp *.cc )

ADD_LIBRARY( vfhdriver SHARED ${SRCS} )
