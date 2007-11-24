INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/libs
)

FILE( GLOB SRCS *.cpp )
GLOBAL_ADD_LIBRARY( OrcaLocalNavUtil SHARED ${SRCS} )

TARGET_LINK_LIBRARIES( OrcaLocalNavUtil HydroNavUtil )
INSTALL_FILES( /include/localnavutil .*\\.h$ )
