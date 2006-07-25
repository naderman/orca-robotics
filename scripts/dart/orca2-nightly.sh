#!/bin/bash

export CVS_RSH=ssh

dashboard=/home/users/dart/ctests/orca/orca2-nightly

# compile with gcc-4.0
logfile=$dashboard/gcc40.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
# alexm: isn't it done as part of ctest'ing?
#rm -rf $dashboard/build-gcc40/* >> $logfile 2>&1
# build and test
/usr/bin/ctest -S $dashboard/orca2-nightly-linux-gcc40.cmake -V >> $logfile 2>&1
# after testing, install so other nightly tests will work
cd $dashboard/build-gcc40 >> $logfile 2>&1
sudo make install >> $logfile 2>&1

# compile with gcc-4.1
logfile=$dashboard/gcc41.log
echo ---------------------------------------- >> $logfile
date >> $logfile
echo ---------------------------------------- >> $logfile
#rm -rf $dashboard/build-gcc41/* >> $logfile 2>&1
# build and test
/usr/bin/ctest -S $dashboard/orca2-nightly-linux-gcc41.cmake -V >> $logfile 2>&1
