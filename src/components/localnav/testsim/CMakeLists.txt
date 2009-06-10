include_directories(
  ${PROJECT_SOURCE_DIR}/src/components/localnav
)

include( ${HYDRO_USE_FILE} )

file( GLOB srcs *.cpp )

set( lib_version ${GBX_PROJECT_VERSION} )

GBX_ADD_LIBRARY( OrcaLocalNavTestSim DEFAULT ${lib_version} ${srcs} )

target_link_libraries( OrcaLocalNavTestSim OrcaObjectUtil OrcaIfaceImpl 
        OrcaOgMap OrcaPathPlan HydroGeom2d )
