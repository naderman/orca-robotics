INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

INCLUDE( ${HYDRO_CMAKE_DIR}/UseHydro.cmake )

FILE( GLOB srcs *.cpp )
GBX_ADD_LIBRARY( OrcaLocalNavTestSim DEFAULT ${srcs} )
TARGET_LINK_LIBRARIES( OrcaLocalNavTestSim OrcaObjectUtil OrcaIfaceImpl 
        OrcaOgMap OrcaPathPlan HydroGeom2d )
