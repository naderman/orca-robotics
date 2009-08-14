include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )

add_library( HydroDllTestOne SHARED myimplementation.cpp )
add_library( HydroDllTestMulti SHARED multiimplementation.cpp )

link_libraries( HydroDll )

add_executable( dlltestone dlltestone.cpp )
GBX_ADD_TEST( DllTestOne           dlltestone )

add_executable( dlltestmulti dlltestmulti.cpp )
GBX_ADD_TEST( DllTestMulti           dlltestmulti )
