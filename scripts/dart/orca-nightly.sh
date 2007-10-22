#!/bin/bash

dashboard=$HOME/ctests/orca/orca-nightly

# compile with gcc-4.2
logfile=$dashboard/orca.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
# build and test
/usr/bin/ctest -S $dashboard/orca-nightly-linux-gcc42.cmake -V >> $logfile 2>&1
# after testing, install so orca-dependent nightly tests will work
cd $dashboard/build-orca >> $logfile 2>&1
make install >> $logfile 2>&1

