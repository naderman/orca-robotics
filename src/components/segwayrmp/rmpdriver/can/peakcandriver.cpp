
#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>   //read and write to ports
#include <string.h>
#include <iostream>

#include <orcaice/context.h>
#include <libpcan.h>  //stuff to access the peak can drivers
#include <pcan.h>

#include <orcaice/context.h>
#include "peakcandriver.h"

using namespace std;
using namespace orcacan;
using namespace segwayrmp;

//********************************************************
PeakCanDriver::PeakCanDriver(const orcaice::Context & context ){
    
    int RetVal=0;
    char TextString[VERSIONSTRING_LEN];
    
    //open the CAN port and use as a chardev type end point
    port_handle = LINUX_CAN_Open("/dev/pcan40",O_RDWR);
    
    if(port_handle){
        cout << "CAN port opened correctly\n";

        // get version info
        RetVal = CAN_VersionInfo(port_handle,TextString);
        if (!RetVal){
            printf("Using Peak can driver:- version = %s\n", TextString);
        }else{
            printf("Peak can driver: failed to get version info\n");
        }

    }else{
        cout<<"Failed to open CAN port correctly\n";
    }

    //**??** We should be setting the initalisation here
    //CAN_Init()...
    //What about CAN_MsgFilter???
}



//********************************************************
PeakCanDriver::~PeakCanDriver(){
    if(port_handle){
        CAN_Close(port_handle);
	//**??** unset the port_handle if successful close
        //**??** revist this. Is there more to do?
    }
}


//*******************************************************
int PeakCanDriver::read(SegwayRmpData& data, std::string &status ){
 
  const int timeOutMicroSeconds=1000;
  DWORD retval = -1;

  //Using data structs from pcan.h
  TPCANRdMsg CanDataReceived;
   
  
  //**??** error handling???
  if( LINUX_CAN_Read_Timeout(port_handle, &CanDataReceived, timeOutMicroSeconds ) == 0){
    cout<<"Data received ID"<<CanDataReceived.Msg.ID <<"Type "<<CanDataReceived.Msg.MSGTYPE<< "\n";
  }else{
    cout<<"Data Rx timed out\n";
  }
}

//*******************************************************
void PeakCanDriver::write(const SegwayRmpCommand& command ){
    // **??** implement this function
    cout<<"this would have been written "<<Message;
}






