include( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )

link_libraries( HydroWifi )

add_executable( wifi_test wifi_test.cpp )
GBX_ADD_TEST( WifiUtilWifiTest wifi_test )
