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


#include <orcaice/orcaice.h>
#include <libpcan.h>  //stuff to access the peak can drivers
#include <pcan.h>

#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>
#include <rmpexception.h>
#include "peakcandriver.h"

using namespace std;
using namespace segwayrmp;

//**??** DO I need some form of MUTEX to lock the access to the port

// ********************************************************
PeakCanDriver::PeakCanDriver(const string & portName ){    

   
    //open the CAN port and use as a chardev type end point
    portHandle_ = LINUX_CAN_Open(portName.c_str() ,O_RDWR);
    isEnabled_ = false;    
 
    if( ! portHandle_ ){
        stringstream ss;
        ss << "PeakCanDriver::constructor(): Error: "<<
            "Unable to open the can port " << portName << endl;
        throw RmpException( ss.str() );
   }else{
        cout << "Can port opened properly\n";
        isEnabled_ = true;
   }

}



// ********************************************************
PeakCanDriver::~PeakCanDriver(){
    if(portHandle_){
      CAN_Close(portHandle_);  //close access to the PCMCIA port
      portHandle_ = NULL;      //clear the port handle
      isEnabled_ = false;
    }
}


// *******************************************************
// Function to read a CAN packet from the card. Will
// returns NO_DATA in the event of a timeout or a failed
// read. Will wait for 10msec (=> 100Hz) for data before 
// timing out

RmpIo::RmpIoStatus 
PeakCanDriver::readPacket( CanPacket* pkt ){
    DWORD RetVal=0;

    //Set the default timeout to 100Hz
    const int timeOutMicroSeconds=10000;

    assert ( isEnabled_ );

    RmpIo::RmpIoStatus status = NO_DATA;

    //Using data structs from pcan.h
    TPCANRdMsg canDataReceived;
     
    //Call the peak library code
    RetVal = LINUX_CAN_Read_Timeout(portHandle_, &canDataReceived, timeOutMicroSeconds ); 

    if ( RetVal == 0 ){
        convertPeakToCanPacket(pkt, &canDataReceived.Msg);
        status = OK;
    }else{        
        if( debugLevel_ > 0)
        { printf("peakcandriver::readPacket(): Data Rx timed out, returned 0x%04x\n",RetVal); }  
        status = NO_DATA;
    }
  
    return status;
}



// ***********************************************************************
// Attempt to write a packet to the CAN card
// throws an Exception if the write fails.

void PeakCanDriver::writePacket( CanPacket* pktToSend ){
    int RetVal = 0; 

    assert ( isEnabled_ );

    //We are supposed to be able to write data at ~ 100Hz, allow a timeout equivalent to 
    //50Hz by default
    const int timeOutMicroSeconds = 20000;  
    
    //Using data structs from pcan.h
    TPCANMsg canDataToSend;
    
    //convert CanPacket to peak Can structure
    convertCanPacketToPeak( &canDataToSend, pktToSend );

    //Send to the peak device driver code
    RetVal = LINUX_CAN_Write_Timeout(portHandle_,  &canDataToSend, timeOutMicroSeconds);

    if( RetVal != 0){
        //The send data failed!
        stringstream ss;
        ss << endl << " --> Attempted write data to CAN card failed: Call Returned "<< RetVal << endl;
        throw RmpException( ss.str() );
    }

}


// ***********************************************************************
// Setup the connection to the CAN card, default baud rates etc.
void 
PeakCanDriver::enable( int debugLevel ){
    DWORD RetVal = 0;

    stringstream ss;

    char TextString[VERSIONSTRING_LEN];
    debugLevel_ = debugLevel;
    
    isEnabled_ = false; //until we get told otherwise!
    
    if(debugLevel_ > 0){
        cout << "TRACE(peakcandriver.cpp): enable()" << endl;

        // get version info from the driv
        if ( CAN_VersionInfo(portHandle_,TextString) == 0 ){
            cout << "Using Peak can driver:- version = " << TextString << endl ;
        }else{
            cout << "Peak can driver: failed to get version info\n";
        }
    }
    
    //Force the default initialisation state of the CAN card. This should happen anyway
    //But let's be explicit about it. We are set to 500K baud and extended can message type...
    //Note in reality we are only ever dealing with standard not extended message types, but the
    //writes barf if we set ST message type here
    RetVal = CAN_Init(portHandle_, CAN_BAUD_500K, CAN_INIT_TYPE_EX );
    
    if( RetVal != 0 ){
        ss << endl << " --> Attempted initialisation of the CAN card failed: Call Returned "<< RetVal << endl;
        throw RmpException( ss.str() );      
    }

 
    //Potentially we could call the CAN_MsgFilter() from libpcan here to filter out some
    //of the un-needed packets (ie heartbeat messages) at a low level. However I haven't
    //been able to get this to work!
   
    isEnabled_ = true; 

    return;
}



// ************************************************************************
// This function is just a place holder, and does very little 
void 
PeakCanDriver::disable( void ){

    if(debugLevel_ > 0){
        cout << "TRACE(peakcandriver.cpp): disable()" << endl;
    }
    
    isEnabled_ = false;
    return;
}



// ***********************************************************************
void 
PeakCanDriver::convertCanPacketToPeak(TPCANMsg *peakCanOut, const CanPacket* pktIn ){

    peakCanOut->ID      =  pktIn->id;
    peakCanOut->LEN     =  CanPacket::CAN_DATA_SIZE;
    memcpy(peakCanOut->DATA, pktIn->msg, CanPacket::CAN_DATA_SIZE * sizeof (BYTE));

    //Use the peak driver definition for a standard (not extended type message)
    //Note that this does not match that used in the FDTI library (see below)
    peakCanOut->MSGTYPE =  MSGTYPE_STANDARD; 

}

// ***********************************************************************
void 
PeakCanDriver::convertPeakToCanPacket(CanPacket* pktOut, const TPCANMsg *peakCanIn ){

    pktOut -> id      =  peakCanIn -> ID;
    memcpy(pktOut -> msg, peakCanIn -> DATA, CanPacket::CAN_DATA_SIZE * sizeof(BYTE));

    //NOTE:- We do not change the value in the flags field. It seems the 
    //different drivers peak / FDTI etc have different definitions of how a standard
    //(not extended) can message is denoted.
    
    /* These two never need to change and are defined at initalisation
      pktOut -> flags   =  pktOut -> flags; 
      pktOut -> dlc     =  CAN_DATA_SIZE;
    */

}


