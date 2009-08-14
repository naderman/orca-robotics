include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
include( ${ORCA_CMAKE_DIR}/UseIceUtil.cmake )

# Test for correct linking.
add_executable( testcan_linktest linktest.cpp )
target_link_libraries( testcan_linktest HydroSegwayRmpAcfrCan )
        