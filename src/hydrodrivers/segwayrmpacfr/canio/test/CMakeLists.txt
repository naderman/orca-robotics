include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

#Now tell the system where to find the libraries we need to link
include_directories( ${PROJECT_SOURCE_DIR}/src/hydrodrivers/segwayrmpacfr)

#Tell the system to build a standalone app for us to use for testing
GBX_ADD_EXECUTABLE( rmptestcan main.cpp )

#The target needs to be linked with these libraries
target_link_libraries( rmptestcan HydroSegwayRmpAcfrCan HydroIceUtil GbxIceUtilAcfr HydroTime )
