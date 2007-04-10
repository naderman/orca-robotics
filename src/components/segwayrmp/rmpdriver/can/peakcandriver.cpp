//******************************************************************
// Written by Duncan Mercer ACFR March 2007 
//********************************************************************
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <ctime>

#include <orcaice/orcaice.h>
#include <libpcan.h>  //stuff to access the peak can drivers
#include <pcan.h>

#include <orcaice/timer.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>
#include <rmpexception.h>
#include "peakcandriver.h"

using namespace std;
using namespace segwayrmp;


//****************************************************************
//A crappy simple method of preventing multiple class instances
//Do Not access this counter directly!!!
//**??** This needs to be handled more robustly!!
namespace caninstancelock{
    static int canInstanceCount = 0;
}

//**************************************************************
//Take the name of the port and attempt to open it, throws an
//exception if fails.
PeakCanDriver::PeakCanDriver(const string & portName ):
    isEnabled_(false), debugLevel_(0)
{    

    //Make sure that that we are the only copy of class that is built
    assert( caninstancelock::canInstanceCount == 0 );
    caninstancelock::canInstanceCount++;


    //open the CAN port and use as a chardev type end point
    portHandle_ = LINUX_CAN_Open(portName.c_str() ,O_RDWR); 

    if( ! portHandle_ ){
        stringstream ss;
        ss << "PeakCanDriver::constructor(): Error: "<<
            "Unable to open the can port. Status returned" << portName << endl;
        throw RmpException( ss.str() );
   }else{
        cout << "Can port opened properly\n";
   }

 }



//*************************************************************
PeakCanDriver::~PeakCanDriver(){
    DWORD RetVal = 0;
    stringstream ss;

    if(portHandle_){
      RetVal = CAN_Close(portHandle_);  //close access to the PCMCIA port
      if(RetVal != 0 ){
          ss << "PeakCanDriver::destructor(): Error: "<<
              "Unable to close can port. " << peakStatusToString(RetVal) << endl;
          throw(RmpException( ss.str()));
      }
      portHandle_ = NULL;      //clear the port handle
      isEnabled_ = false;
    }
    caninstancelock::canInstanceCount--; 
}


//*************************************************************
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
        if( debugLevel_ > 0){ 
            cout << "peakcandriver::readPacket(): Data Rx timed out, call returned "
                 << peakStatusToString(RetVal) << endl;
        }  
        status = NO_DATA;
    }
  
    return status;
}





//***********************************************************************
// Attempt to write a packet to the CAN card
// throws an Exception if the write fails.

void PeakCanDriver::writePacket( CanPacket* pktToSend ){
    DWORD RetVal = 0; 

    assert ( isEnabled_ );

    /*  //debug
        double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
        writeTimer_.restart();
        cout << "Sending " << pktToSend->toString()<<"OrcaTimer msec " << msecs <<endl;
    */

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
        ss << endl << " --> Attempted write data to CAN card failed: Call Returned "<< 
            peakStatusToString(RetVal) << endl;
        throw RmpException( ss.str() );
    }

}


//***********************************************************************
// Setup the connection to the CAN card, default baud rates etc.
void 
PeakCanDriver::enable( int debugLevel ){
    DWORD RetVal = 0;

    stringstream ss;

    char TextString[VERSIONSTRING_LEN];
    debugLevel_ = debugLevel;
        
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
        ss << endl << " --> Attempted initialisation of the CAN card failed: Error Returned " << 
            peakStatusToString(RetVal )<< endl;
        throw RmpException( ss.str() );      
    }

 
    //Potentially we could call the CAN_MsgFilter() from libpcan here to filter out some
    //of the un-needed packets (ie heartbeat messages) at a low level. However I haven't
    //been able to get this to work!
   
    isEnabled_ = true; 

    return;
}



//************************************************************************
// This function is just a place holder, and does very little 
void 
PeakCanDriver::disable( void ){
    DWORD RetVal=0;

    if(debugLevel_ > 0){
        RetVal = CAN_Status(portHandle_);
        cout << "TRACE(peakcandriver.cpp): disable() CAN Status:- " <<
           peakStatusToString(RetVal) << endl;
    }
    
    isEnabled_ = false;
    return;
}



//***********************************************************************
void 
PeakCanDriver::convertCanPacketToPeak(TPCANMsg *peakCanOut, const CanPacket* pktIn ){

    peakCanOut->ID      =  pktIn->id;
    peakCanOut->LEN     =  CanPacket::CAN_DATA_SIZE;
    memcpy(peakCanOut->DATA, pktIn->msg, CanPacket::CAN_DATA_SIZE * sizeof (BYTE));

    //Use the peak driver definition for a standard (not extended type message)
    //Note that this does not match that used in the FDTI library (see below)
    peakCanOut->MSGTYPE =  MSGTYPE_STANDARD; 

}

//***********************************************************************
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

//*****************************************************************************

std::string 
PeakCanDriver::peakStatusToString (DWORD status){
    switch (status)
    {
    case CAN_ERR_OK: 
        return "CAN_ERR_OK";       //no error
    case CAN_ERR_XMTFULL:
        return "CAN_ERR_XMTFULL";  // transmit buffer full
    case CAN_ERR_OVERRUN:
        return "CAN_ERR_OVERRUN";   // overrun in receive buffer
    case CAN_ERR_BUSLIGHT:
        return "CAN_ERR_BUSLIGHT";  // bus error, errorcounter limit reached
    case CAN_ERR_BUSHEAVY:
        return "CAN_ERR_BUSHEAVY";   // bus error, errorcounter limit reached
    case CAN_ERR_BUSOFF:
        return "CAN_ERR_BUSOFF";     // bus error, 'bus off' state entered
    case CAN_ERR_QRCVEMPTY:
        return "CAN_ERR_QRCVEMPTY";  // receive queue is empty
    case CAN_ERR_QOVERRUN:
        return " CAN_ERR_QOVERRUN:"; // receive queue overrun
    case CAN_ERR_QXMTFULL:
        return "CAN_ERR_QXMTFULL";   // transmit queue full 
    case CAN_ERR_REGTEST:
        return "CAN_ERR_REGTEST";    // test of controller registers failed
    case CAN_ERR_NOVXD:
        return "CAN_ERR_NOVXD";       // Win95/98/ME only
    case CAN_ERR_RESOURCE:
        return "CAN_ERR_RESOURCE";      // can't create resource
    case CAN_ERR_ILLPARAMTYPE:
        return "CAN_ERR_ILLPARAMTYPE";  // illegal parameter
    case CAN_ERR_ILLPARAMVAL:         
        return "CAN_ERR_ILLPARAMVAL";   // value out of range
    case CAN_ERRMASK_ILLHANDLE:
        return "CAN_ERRMASK_ILLHANDLE"; // wrong handle, handle error
    default:
        return "CAN status unknown!";
    }

}
