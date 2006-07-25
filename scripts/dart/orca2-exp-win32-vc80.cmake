#
# Edit this to match your configuration, then set a cron job
# to run it regularly (with 'ctest -S <script_name>').
#
# NOTE: to install from command line run this:
# >"C:\Program Files\Microsoft Visual Studio 8\Common7\IDE\devenv.com" build-vc80\orca.sln /build Debug /project INSTALL
#
SET (CTEST_DASHBOARD_ROOT "C:/ctests/orca/orca2-exp")
SET (CTEST_SOURCE_DIRECTORY "${CTEST_DASHBOARD_ROOT}/orca2")
SET (CTEST_BINARY_DIRECTORY "${CTEST_DASHBOARD_ROOT}/build-vc80")

SET (CTEST_CVS_COMMAND 	"C:/Program Files/TortoiseCVS/cvs.exe")
#SET (CTEST_CVS_CHECKOUT	"${CTEST_CVS_COMMAND} -d :ext:orcacvs-session:/cvsroot/orca-robotics co -d \"${CTEST_SOURCE_DIRECTORY}\" orca2")

# which command to use for running the dashboard
#
#SET (CTEST_COMMAND "ctest -D Experimental -C Debug -A \"${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}\" --interactive-debug-mode 0" )
SET (CTEST_COMMAND "ctest -D Experimental -C Debug -A \"${CTEST_BINARY_DIRECTORY}/cmake_config_report.txt\" --interactive-debug-mode 0" )

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND cmake )


####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest wipe the binary tree before running
SET ( CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE )

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
SITE:STRING=devpirate
# this is only for Makefile-based build systems
#CMAKE_BUILD_TYPE:STRING=Debug
# in future versions of ctest /build variable may replaced with what's supplied with -C Xxxx option
MAKECOMMAND:STRING=\"C:/Program Files/Microsoft Visual Studio 8/Common7/IDE/devenv.com\" orca.sln /build Debug /project ALL_BUILD
")

# set any extra envionment variables here
SET (CTEST_ENVIRONMENT
)
