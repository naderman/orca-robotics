#!/bin/bash

export ORCA2_INSTALL=/opt/orca2-cont

dashboard=/home/users/dart/ctests/orca/orca2-cont
logfile=$dashboard/orca2-cont-gcc40.log

while true; do
 
 # orca
 echo ---------- ORCA ------------------- >> $logfile
 date >> $logfile
 echo ----------------------------------- >> $logfile
 
 echo ===== Building and Testing ===== >> $logfile
 /usr/bin/ctest -S $dashboard/orca2-cont-linux-gcc40.cmake -VV >> $logfile 2>&1
 
 echo ===== Installing =============== >> $logfile
 cd $dashboard/build-gcc40 >> $logfile 2>&1
 sudo make install >> $logfile 2>&1
 cd ~ >> $logfile 2>&1
 echo ===== Finished  =============== >> $logfile
  
 sleep 311

done
