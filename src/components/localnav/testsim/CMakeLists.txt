INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

FILE( GLOB SRCS *.cpp )
GLOBAL_ADD_LIBRARY( OrcaLocalNavTestSim SHARED ${SRCS} )
TARGET_LINK_LIBRARIES( OrcaLocalNavTestSim OrcaObjectUtil OrcaIfaceImpl 
        OrcaOgMap OrcaPathPlan HydroGeom2d )
