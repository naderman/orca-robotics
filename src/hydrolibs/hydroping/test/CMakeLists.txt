include_directories( ${PROJECT_SOURCE_DIR}/src/libs )

add_executable( pinglatencymstest pinglatencymstest.cpp )
target_link_libraries( pinglatencymstest HydroPing )
#
# Can't perform this test because it requires root priveliges...
#
#GBX_ADD_TEST( PingLatencyMsTest pinglatencymstest )
