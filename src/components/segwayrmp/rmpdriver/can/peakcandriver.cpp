//******************************************************************
// Written by Duncan Mercer ACFR March 2007 
//********************************************************************
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

#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>
#include "peakcandriver.h"

using namespace std;
using namespace segwayrmp;

//********************************************************
PeakCanDriver::PeakCanDriver(const orcaice::Context & context ){
    
    char TextString[VERSIONSTRING_LEN];
    
    //**??** how do I generalise this????
    //open the CAN port and use as a chardev type end point
    portHandle_ = LINUX_CAN_Open("/dev/pcan40",O_RDWR);
    
 
    //**??** what about throwing exceptions here ?????
   if( portHandle_ ){
        cout << "CAN port opened correctly\n";
        
        // get version info from the driver
        if ( CAN_VersionInfo(portHandle_,TextString) == 0 ){
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
    if(portHandle_){
      CAN_Close(portHandle_);  //close access to the PCMCIA port
      portHandle_ = NULL;      //clear the port handle
        //**??** revist this. Is there more to do?
    }
}


//*******************************************************
RmpIo::RmpIoStatus 
PeakCanDriver::readPacket( CanPacket* pkt ){

    const int timeOutMicroSeconds=1000;

    //**??** is the assert the right choice of error?
    assert ( portHandle_ != NULL );

    RmpIo::RmpIoStatus status = NO_DATA;

    //Using data structs from pcan.h
    TPCANRdMsg canDataReceived;
     
    if( LINUX_CAN_Read_Timeout(portHandle_, &canDataReceived, timeOutMicroSeconds ) == 0){

        //**??** can we get rid of this step???
        convertPeakToCanPacket(pkt, &canDataReceived.Msg);
        status = OK;
    }else{
        //**??** error handling????
        cout<<"Data Rx timed out\n";
        status = NO_DATA;
    }
  

    return status;
}



//***********************************************************************
void PeakCanDriver::writePacket( CanPacket* pktToSend ){
    int RetVal = 0; 

    //We are supposed to be able to write data at ~ 100Hz, allow a timeout of 50Hz by default
    const int timeOutMicroSeconds = 20000;  
    
    //Used for error notification
    stringstream ss;

    //**??** is the throw the right choice of error? What if no listeners?
    assert ( portHandle_ != NULL );

    //Using data structs from pcan.h
    TPCANMsg canDataToSend;
    
    //**??** can we get rid of this step???
    //convert CanPacket to peak Can structure
    convertCanPacketToPeak( &canDataToSend, pktToSend );

    //Send to the peak device driver code
    RetVal = LINUX_CAN_Write_Timeout(portHandle_,  &canDataToSend, timeOutMicroSeconds);
    if( RetVal != 0){
        //The send data failed!
        
        //**??** sensible error handler ???
        stringstream ss;
        ss << endl << " --> Attempting to write to the peak CAN card failed: Returned "<<RetVal <<endl;
        throw RmpException( ss.str() );
    }

}


//***********************************************************************
void 
PeakCanDriver::enable( int debugLevel ){
    debugLevel_ = debugLevel;

    if(debugLevel_ > 0){
        cout<<"TRACE(peakcandriver.cpp): enable()" << endl;
    }

    //**??** finish me off!
    return;


  
}

//************************************************************************

void 
PeakCanDriver::disable( void ){
  //**??** finish me off!
    if(debugLevel_ > 0){
        cout<<"TRACE(peakcandriver.cpp): disable()" << endl;
    }

    return;
}



//***********************************************************************
void 
PeakCanDriver::convertCanPacketToPeak(TPCANMsg *peakCanOut, const CanPacket* pktIn ){

    peakCanOut->ID      =  pktIn->id;
    peakCanOut->MSGTYPE =  pktIn->flags; //**??** Is this the correct translation????
    peakCanOut->LEN     =  (uint8_t) pktIn->dlc;
    memcpy(peakCanOut->DATA, pktIn->msg, CanPacket::CAN_DATA_SIZE);

}

//***********************************************************************
void 
PeakCanDriver::convertPeakToCanPacket(CanPacket* pktOut, const TPCANMsg *peakCanIn ){

    pktOut -> id      =  peakCanIn -> ID;
    pktOut -> flags   =  peakCanIn -> MSGTYPE; //**??** Is this the correct translation????
    pktOut -> dlc     =  peakCanIn -> LEN;
    memcpy(pktOut -> msg, peakCanIn -> DATA, CanPacket::CAN_DATA_SIZE * sizeof(BYTE));

}
