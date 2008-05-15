#!/bin/bash

# set timezone to UTC
export TZ="UTC"

# dashboard 'home'
dashboard=$HOME/ctests/orca/orca-nightly

# required for prop tests
export LD_LIBRARY_PATH=$HOME/ctests/orca/orca-nightly/build-orca/src/libs/orcaice/test:$LD_LIBRARY_PATH

# build and test
logfile=$dashboard/orca.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
/usr/bin/ctest -S $dashboard/orca-nightly-linux-gcc42.cmake -V >> $logfile 2>&1

# install, so orca-dependent nightly tests will work
cd $dashboard/build-orca >> $logfile 2>&1
make install >> $logfile 2>&1

