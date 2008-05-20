#!/bin/bash

dashboard=/home/users/dart/ctests/orca-and-friends/orca-and-friends-cont
logfile=$dashboard/orca-and-friends.log

export ICE_HOME=/opt/Ice
export PATH=$ICE_HOME/bin:$PATH
export LD_LIBRARY_PATH=$ICE_HOME/lib:$LD_LIBRARY_PATH

export TZ="UTC"

while true; do

 echo ------- ORCA AND FRIENDS ---------- >> $logfile
 date >> $logfile
 echo ----------------------------------- >> $logfile

 echo ===== Building Gearbox  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/gearbox-cont-linux-gcc42.cmake -V >> $logfile 2>&1

 echo ==== Installing Gearbox  ===== >> $logfile
 cd $dashboard/build-gearbox >> $logfile 2>&1
 make install >> $logfile 2>&1
 cd ~ >> $logfile 2>&1

 echo ===== Building Hydro  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/hydro-cont-linux-gcc42.cmake -V >> $logfile 2>&1

 echo ==== Installing Hydro ===== >> $logfile
 cd $dashboard/build-hydro >> $logfile 2>&1
 make install >> $logfile 2>&1
 cd ~ >> $logfile 2>&1

 echo ===== Building and Testing Orca  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/orca-cont-linux-gcc42.cmake -V >> $logfile 2>&1

 echo ===== Installing Orca =============== >> $logfile
 cd $dashboard/build-orca >> $logfile 2>&1
 make install >> $logfile 2>&1
 cd ~ >> $logfile 2>&1

 echo ===== Building and Testing empty-project  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/empty-cont-linux-gcc42.cmake -V >> $logfile 2>&1

 echo ===== Building and Testing shh  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/shh-cont-linux-gcc42.cmake -V >> $logfile 2>&1

 echo ===== Finished  =============== >> $logfile

sleep 311

done
