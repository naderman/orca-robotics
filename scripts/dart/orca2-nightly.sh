#!/bin/bash

export CVS_RSH=ssh

dashboard=$HOME/ctests/orca/orca2-nightly

# compile with gcc-4.0
logfile=$dashboard/gcc40.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
# build and test
/usr/bin/ctest -S $dashboard/orca2-nightly-linux-gcc40.cmake -V >> $logfile 2>&1

# install so other nightly tests will work
cd $dashboard/build-gcc40 >> $logfile 2>&1
sudo make install >> $logfile 2>&1

# compile with gcc-4.1
logfile=$dashboard/gcc41.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
# build and test
/usr/bin/ctest -S $dashboard/orca2-nightly-linux-gcc41.cmake -V >> $logfile 2>&1
