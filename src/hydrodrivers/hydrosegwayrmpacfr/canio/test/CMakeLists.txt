include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

include_directories( ${PROJECT_SOURCE_DIR}/src/hydrodrivers/hydrosegwayrmpacfr)
GBX_ADD_EXECUTABLE( rmptestcan rmptestcan.cpp )
target_link_libraries( rmptestcan HydroSegwayRmpAcfrCan HydroIceUtil GbxIceUtilAcfr HydroTime )

GBX_ADD_EXECUTABLE( testreadfromcan testreadfromcan.cpp )
target_link_libraries( testreadfromcan pcan GbxUtilAcfr HydroSegwayRmpAcfrCan )

GBX_ADD_EXECUTABLE( canpeakrmpiotest canpeakrmpiotest.cpp )
target_link_libraries( canpeakrmpiotest pcan GbxUtilAcfr HydroSegwayRmpAcfrCan )
