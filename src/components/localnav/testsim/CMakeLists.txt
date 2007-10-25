INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

FILE( GLOB SRCS *.cpp )
GLOBAL_ADD_LIBRARY( OrcaLocalNavTestSim SHARED ${SRCS} )
INSTALL_TARGETS( /lib OrcaLocalNavTestSim )
TARGET_LINK_LIBRARIES( OrcaLocalNavTestSim OrcaOgMap OrcaObjectUtil OrcaIfaceImpl OrcaPathPlan )
