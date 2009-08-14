include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroGeom2d HydroUtil  )

add_executable( point point.cpp )
GBX_ADD_TEST( HydroGeom2dPoint point )

add_executable( circle circle.cpp )
GBX_ADD_TEST( HydroGeom2dCircle circle )

add_executable( line line.cpp )
GBX_ADD_TEST( HydroGeom2dLine line )

