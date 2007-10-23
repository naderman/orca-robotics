INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

LINK_LIBRARIES( OrcaWifi )

ADD_EXECUTABLE( wifi_test wifi_test.cpp )
GLOBAL_ADD_TEST( WifiUtilWifiTest wifi_test )
