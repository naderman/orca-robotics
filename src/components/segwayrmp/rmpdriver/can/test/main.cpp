
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>   //read and write to ports
#include <string.h>
#include <iostream>

#include <libpcan.h>  //stuff to access the peak can drivers

#include <orcaice/context.h>
#include <rmpdriver/rmpio.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/can/peakcandriver.h>


using namespace std;
using namespace segwayrmp;

int main(void){


    segwayrmp::CanPacket dataPacket;
    std::string portName("/dev/pcan40");

    //declare our Can Driver object
    segwayrmp::PeakCanDriver test( portName );
    test.enable(4);
 
    //**??** a bogus command???
    test.writePacket(&dataPacket);
    
    
    while(true){ //**??** how should this be escaped?
      //Keep trying to read data from the CAN interface
      if ( test.readPacket(&dataPacket) == RmpIo::OK )
        { cout<< dataPacket.toString() << endl; }
    }
      
  return 0;

}

