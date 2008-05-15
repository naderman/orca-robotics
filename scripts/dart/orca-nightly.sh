#!/bin/bash

export TZ="UTC"
dashboard=$HOME/ctests/orca/orca-nightly

# required for prop tests
export LD_LIBRARY_PATH=$HOME/ctests/orca/orca-nightly/build-orca/src/libs/orcaice/test:$LD_LIBRARY_PATH

# compile with gcc-4.2
logfile=$dashboard/orca.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile

# make clean first
#cd $dashboard/build-orca >> $logfile 2>&1
#make clean

# build and test
/usr/bin/ctest -S $dashboard/orca-nightly-linux-gcc42.cmake -V >> $logfile 2>&1

# after testing, install so orca-dependent nightly tests will work
cd $dashboard/build-orca >> $logfile 2>&1
make install >> $logfile 2>&1

