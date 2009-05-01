include_directories(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

include( ${HYDRO_USE_FILE} )

file( GLOB srcs *.cpp )
GBX_ADD_LIBRARY( OrcaLocalNavTestSim DEFAULT ${srcs} )
target_link_libraries( OrcaLocalNavTestSim OrcaObjectUtil OrcaIfaceImpl 
        OrcaOgMap OrcaPathPlan HydroGeom2d )
