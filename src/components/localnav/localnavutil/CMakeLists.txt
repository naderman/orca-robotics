INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/libs
)

FILE( GLOB srcs *.cpp )
GLOBAL_ADD_LIBRARY( OrcaLocalNavUtil SHARED ${srcs} )

TARGET_LINK_LIBRARIES( OrcaLocalNavUtil HydroNavUtil )
INSTALL_FILES( /include/localnavutil .*\\.h$ )
