include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

# to include driver files
include_directories( ${PROJECT_SOURCE_DIR}/src/hydrodrivers/hydrosegwayrmpacfr)

#Tell the system to build a standalone app for us to use for testing
GBX_ADD_EXECUTABLE( rmptestcan main.cpp )

#The target needs to be linked with these libraries
target_link_libraries( rmptestcan HydroSegwayRmpAcfrCan HydroIceUtil GbxIceUtilAcfr HydroTime )
