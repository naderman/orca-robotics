include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroMapLoad HydroUtil )

add_executable( ogloadsave ogloadsave.cpp )
GBX_ADD_TEST( HydroMapLoadOgLoadSave ogloadsave )
