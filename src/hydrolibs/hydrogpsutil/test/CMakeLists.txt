include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroGpsUtil )

add_executable( testmga testmga.cpp )
GBX_ADD_TEST( HydroGpsUtilTestMga testmga )
