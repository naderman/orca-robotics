include_directories( ${PROJECT_SOURCE_DIR}/src/libs )

add_executable( pingtest pingtest.cpp )
target_link_libraries( pingtest oping HydroPing )
#
# Can't perform this test because it requires root priveliges...
#
#GBX_ADD_TEST( PingTest pingtest "localhost" )


#
# AlexB: A test that seems to indicate an error.
#        Double-check when I'm not so tired before submitting bug report.
#
add_executable( opingerror opingerror.cpp )
target_link_libraries( opingerror oping HydroPing )
