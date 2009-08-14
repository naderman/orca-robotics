include( ${HYDRO_CMAKE_DIR}/UseBasicRules.cmake )
include( ${HYDRO_CMAKE_DIR}/UseHydroUtil.cmake )

add_executable( stringtest stringtest.cpp )
GBX_ADD_TEST( StringTest stringtest )

add_executable( systest systest.cpp )
GBX_ADD_TEST( SysTest systest )

add_executable( randtest randtest.cpp )
GBX_ADD_TEST( RandTest randtest )

add_executable( propertiestest propertiestest.cpp )
GBX_ADD_TEST( PropertiesTest propertiestest )
