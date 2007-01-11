INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

ADD_EXECUTABLE( wifi_test wifi_test.cpp )
TARGET_LINK_LIBRARIES( wifi_test OrcaWifi )

## Tests ##
#ADD_TEST( WifiUtilWifiTest wifi_test )
#MESSAGE( STATUS "Planning to Build Test    : WifiUtilWifiTest [OrcaWifiUtil]")
