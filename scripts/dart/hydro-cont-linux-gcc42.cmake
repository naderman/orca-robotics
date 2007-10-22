#
# Edit this to match your configuration, then set a cron job
# to run it regularly (with 'ctest -S <script_name>').
#
SET (CTEST_DASHBOARD_ROOT "/home/users/dart/ctests/orca-and-friends/orca-and-friends-cont")
SET (CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/hydro")
SET (CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/build-hydro")

SET (CTEST_CVS_COMMAND "svn")

# which command to use for running the dashboard
#
#SET (CTEST_COMMAND "ctest -D Continuous -A \"${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}\"" )
SET (CTEST_COMMAND "ctest -D Continuous -A \"${CTEST_BINARY_DIRECTORY}/cmake_config_report.txt\"" )

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND "cmake" )


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
#SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
MAKECOMMAND:STRING=make
BUILDNAME:STRING=Debian-gcc42
SITE:STRING=devdebian.acfr.usyd.edu.au
CMAKE_BUILD_TYPE:STRING=Debug
BUILD_IMAGE_SERVER:BOOL=ON
")

# set any extra envionment variables here
SET (CTEST_ENVIRONMENT
CC=/home/users/dart/bin/gcc
CXX=/home/users/dart/bin/g++
)
