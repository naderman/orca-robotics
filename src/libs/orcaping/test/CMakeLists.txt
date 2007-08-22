INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src/libs )

ADD_EXECUTABLE( pingtest pingtest.cpp )
TARGET_LINK_LIBRARIES( pingtest oping OrcaPing )
#
# Can't perform this test because it requires root priveliges...
#
#ADD_TEST( PingTest pingtest "localhost" )
#MESSAGE ( STATUS "Planning to Build Test      : pingTest [libOrcaPing]")
