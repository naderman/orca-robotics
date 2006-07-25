#!/bin/bash

export ORCA2_INSTALL=/opt/orca2-cont

dashboard=/home/users/dart/ctests/orca-and-friends/orca-and-friends-cont
logfile=$dashboard/orca-and-friends.log

while true; do
 
 # orca
 echo ------- ORCA AND FRIENDS ---------- >> $logfile
 date >> $logfile
 echo ----------------------------------- >> $logfile
 
 echo ===== Building and Testing Orca2  ===== >> $logfile
 /usr/bin/ctest -S $dashboard/orca2-cont-linux-gcc40.cmake -V >> $logfile 2>&1
 
 echo ===== Installing Orca2 =============== >> $logfile
 cd $dashboard/build-orca2 >> $logfile 2>&1
 sudo make install >> $logfile 2>&1
 cd ~ >> $logfile 2>&1

 echo ===== Building and Testing empty-project  ===== >> $logfile
  /usr/bin/ctest -S $dashboard/empty-cont-linux-gcc40.cmake -V >> $logfile 2>&1
  
# echo ===== Building and Testing bruce2  ===== >> $logfile
#   /usr/bin/ctest -S $dashboard/bruce2-cont-linux-gcc40.cmake -V >> $logfile 2>&1
   
# echo ===== Building and Testing dgc  ===== >> $logfile
#   /usr/bin/ctest -S $dashboard/dgc-cont-linux-gcc40.cmake -V >> $logfile 2>&1
   
 echo ===== Finished  =============== >> $logfile
  
 sleep 311

done
