/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <IceUtil/IceUtil.h>

#include "usbftdi.h"

#include "rmpusbdataframe.h"
#include "rmpdefs.h"
#include "canpacket.h"

// magic numbers
#define SEGWAY_USB_VENDOR_ID                    0x0403
#define SEGWAY_USB_PRODUCT_ID                   0xE729
#define SEGWAY_USB_VENDOR_PRODUCT_ID            0x0403E729
#define SEGWAY_USB_DESCRIPTION                  "Robotic Mobile Platform"
#define SEGWAY_USB_MESSAGE_SIZE                 18
#define SEGWAY_USB_MSG_START                    0xF0
#define SEGWAY_USB_HEARTBEAT_MESSAGE            0x0
#define SEGWAY_USB_STATUS_MESSAGE               0x55
#define SEGWAY_USB_CHANNEL_A                    0xAA
#define SEGWAY_USB_CHANNEL_B                    0xBB

using namespace std;
using namespace segwayrmp;


UsbIoFtdi::UsbIoFtdi()
{
    // Set initial buffer size and number of bytes stored
    charBuffer_.resize(128);
    charBufferBytes_ = 0;
    
    pthread_mutex_init(&eventHandle_.eMutex, NULL);
    pthread_cond_init(&eventHandle_.eCondVar, NULL);
}

UsbIoFtdi::~UsbIoFtdi()
{
}

/*
 * Initializes the USB device.
 *
 * returns: 0 on success, non-zero on error
 */
UsbIo::UsbIoStatus
UsbIoFtdi::init()
{
    // custom device settings
    DWORD iVID = SEGWAY_USB_VENDOR_ID;
    DWORD iPID = SEGWAY_USB_PRODUCT_ID;
    // without this the library will not find the segway
    FT_SetVIDPID(iVID, iPID);   // use our VID and PID;

    FT_STATUS ftStatus;
//    FT_HANDLE ftHandleTemp;
//    DWORD numDevs;
//    DWORD Flags;
//    DWORD ID;
//    DWORD Type;
//    DWORD LocId;
//    char SerialNumber[16];
    char Description[64];
    /*
    //
    // create the device information list
    //
    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if ( ftStatus != FT_OK )
    {
        cout<<"couldn't get a list of devices"<<endl;
        return UsbIo::IO_ERROR;
    }
    cout<<"found "<<(int)numDevs<<" devices"<<endl;
    if ( numDevs <= 0 ) {
        return UsbIo::OTHER_ERROR;
    }
    
    // this is the index of segway in the list of USB devices
    int segwayDeviceIndex = -1;
    
    // get information for all devices
    for ( uint i=0; i<numDevs; ++i ) { 
        ftStatus = FT_GetDeviceInfoDetail( i, &Flags, &Type, &ID, &LocId, SerialNumber, Description, &ftHandleTemp );
        if ( ftStatus == FT_OK && ID==SEGWAY_USB_VENDOR_PRODUCT_ID ) {
            cout<<"found Segway device"<<endl;
            segwayDeviceIndex = i;
            
            cout<<"Device :"<<i<<endl;
            cout<<"Flags  :"<<hex<<(int)Flags<<endl;
            cout<<"Type   :"<<hex<<(int)Type<<endl;
            cout<<"ID     :"<<hex<<(int)ID<<endl;
            cout<<"LocId  :"<<hex<<(int)LocId<<endl;
            cout<<"Serial :"<<SerialNumber<<endl;
            cout<<"Descrip:"<<Description<<endl;
            cout<<"Handle :"<<hex<<(int)ftHandleTemp<<endl;

            break;
        }
    }
    
    // didn't find the segway device
    if ( segwayDeviceIndex==-1 ) {
        cout<<"Did not find Segway device. All devices: "<<endl;
        for ( uint i=0; i<numDevs; ++i ) { 
            ftStatus = FT_GetDeviceInfoDetail( i, &Flags, &Type, &ID, &LocId, SerialNumber, Description, &ftHandleTemp );
            cout<<"============================"<<endl;
            cout<<"Device :"<<i<<endl;
            cout<<"Flags  :"<<hex<<(int)Flags<<endl;
            cout<<"Type   :"<<hex<<(int)Type<<endl;
            cout<<"ID     :"<<hex<<(int)ID<<endl;
            cout<<"LocId  :"<<hex<<(int)LocId<<endl;
            cout<<"Serial :"<<SerialNumber<<endl;
            cout<<"Descrip:"<<Description<<endl;
            cout<<"Handle :"<<hex<<(int)ftHandleTemp<<endl;
        }
        return UsbIo::OTHER_ERROR;
    }
    
    ftStatus = FT_Open( segwayDeviceIndex, &ftHandle_ );
    if ( ftStatus != FT_OK) {
        cout<<"FT_Open failed ("<<ftStatus<<")"<<endl;
        return UsbIo::IO_ERROR;
    }
    cout<<"FT_Open OK"<<endl;
    */
    strcpy(Description,SEGWAY_USB_DESCRIPTION);
    ftStatus = FT_OpenEx(Description ,FT_OPEN_BY_DESCRIPTION, &ftHandle_ );
    if ( ftStatus != FT_OK) {
        cout<<"FT_OpenEx failed ("<<ftStatus<<")"<<endl;
        return UsbIo::IO_ERROR;
    }
    cout<<"FT_OpenEx OK"<<endl;

    ftStatus = FT_SetBaudRate( ftHandle_, 460800 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetBaudRate failed ("<<ftStatus<<")"<<endl;
        FT_Close( ftHandle_ );        //close open device
        return UsbIo::IO_ERROR;
    }   
    cout<<"FT_SetBaudRate OK"<<endl;

    //set the latency timer to 2ms (valid range is from 2 to 255 ms)
    ftStatus = FT_SetLatencyTimer( ftHandle_, 2 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetLatencyTimer failed ("<<ftStatus<<")"<<endl;
        //latency not set - but we won't know   return -82;
        return UsbIo::IO_ERROR;
    }    
    cout<<"FT_SetLatencyTimer OK"<<endl;

    // debug
    /*
    CanPacket ppp;
    for ( int i=0; i<10; ++i )
    {
        ReadPacket( &ppp, 0 );
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(50));
    }
    */
    
    //
    // The reset is in a separate function so we call it again later.
    //
    return reset();
}

UsbIo::UsbIoStatus UsbIoFtdi::reset()
{
    assert( ftHandle_ );

    FT_STATUS ftStatus;

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }

    ftStatus = FT_Purge( ftHandle_, FT_PURGE_RX | FT_PURGE_TX);
    if ( ftStatus != FT_OK ) {
        cout<<"FT_Purge failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_Purge OK"<<endl;
    }

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }
    /*
        //extend timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 1000, 1000 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }
    */
    IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(150));

      //normal timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 0, 0 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }

    if ( ftStatus == FT_OK ) {
        return UsbIo::OK;
    }
    else {
        return UsbIo::IO_ERROR;
    }
}
  
/*
 * Closes the USB device
 *
 * returns: 0 on success, negative otherwise
 */
UsbIo::UsbIoStatus UsbIoFtdi::shutdown()
{
    if ( FT_Close( ftHandle_ ) == FT_OK ) {
        return UsbIo::OK;
    }
    else {
        return UsbIo::IO_ERROR;
    }
}

UsbIo::UsbIoStatus
UsbIoFtdi::readPacket(CanPacket* pkt)
{
    UsbIo::UsbIoStatus status = readPacketNonBlocking( pkt );
    // if got a packet or error, return right away...
    if ( status==UsbIo::OK || status<0 ) {
        return status;
    }
    
    // ... if not, try to block
    return readPacketBlocking( pkt );
}
    
UsbIo::UsbIoStatus
UsbIoFtdi::readPacketNonBlocking( CanPacket* pkt )
{
    UsbIo::UsbIoStatus status;
    
    // if there's already a packet in CAN packet buffer, just return it
    status = getPacket( pkt );
    if ( status == UsbIo::OK ) {
        return UsbIo::OK;
    }

    // there's not packet ready to be returned.
    // Try to read from the USB buffer into our own char buffer
    status = readFromUsbToBufferNonBlocking();
    if ( status < 0 ) {
        return status;
    }
    
    // Try to parse buffer contents into CAN packets
    status = readFromBufferToQueue();
    if ( status < 0 ) {
        return status;
    }
    
    // try to return a packet again
    status = getPacket( pkt );
    if ( status == UsbIo::OK ) {
        return UsbIo::OK;
    }

    // did not get a packet, but it's not an error
    return UsbIo::NO_DATA;
}

UsbIo::UsbIoStatus
UsbIoFtdi::readPacketBlocking( CanPacket* pkt )
{
    UsbIo::UsbIoStatus status;
    
    // arbitrary parameter
    int blockCount = 5;
    
    while ( blockCount )
    {
        --blockCount;

        // this blocking call uses a timed conditional variable, so it shouldn't lock up forever
        status = readFromUsbToBufferBlocking();
        if ( status < 0 ) {
            return status;
        }
        
        // Try to parse buffer contents into CAN packets
        status = readFromBufferToQueue();
        if ( status < 0 ) {
            return status;
        }
        
        // try to return a packet
        status = getPacket( pkt );
        if ( status == UsbIo::OK ) {
            return UsbIo::OK;
        }
    }
    
    // did not get a packet, but it's not an error
    return UsbIo::NO_DATA;
}

// this function is not currently used.
// Use readFromUsbToBufferBlocking(), it is more efficient.
UsbIo::UsbIoStatus
UsbIoFtdi::readPacketPolling( CanPacket* pkt )
{
    UsbIo::UsbIoStatus status;
    status = getPacket( pkt );
    if ( status == UsbIo::OK ) {
        return UsbIo::OK;
    }

    // made up variables
    const int pollingIntervalUsec = 20000;
    int pollCount = 10;

    while ( pollCount )
    {
        --pollCount;

        // Read from the USB buffer into our own buffer
        uint bytesInBuffer = charBufferBytes_;
        
        status = readFromUsbToBufferNonBlocking();
        if ( status < 0 ) {
            return status;
        }
        
        // if the number of bytes in the buffer hasn't changed, try again at 50Hz
        if ( charBufferBytes_ == bytesInBuffer ) {
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(pollingIntervalUsec));
            continue;
        }
    
        // Try to parse buffer contents into CAN packets
        status = readFromBufferToQueue();
        if ( status < 0 ) {
            return status;
        }
        
        // try to return a packet
        status = getPacket( pkt );
        if ( status == UsbIo::OK ) {
            return UsbIo::OK;
        }
    }

    // did not get a packet, but it's not an error
    return UsbIo::NO_DATA;
}
  
/*
 * Writes the given packet to USB.
 *
 * returns: 0 on success, negative error code otherwise
 */
UsbIo::UsbIoStatus
UsbIoFtdi::writePacket( CanPacket *pkt )
{
    /*
    // debug
    DWORD bytesInRxUsb, bytesInTxUsb, usbEvent;
    FT_STATUS ftStatusB = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );    
    if ( ftStatusB == FT_OK )  {
        cout<<"Status before write: rx: "<<bytesInRxUsb<<endl;
        //cout<<"Status on write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    ftStatusB = FT_GetStatus( ftHandle_, &bytesInRxUsb, &bytesInTxUsb, &usbEvent );
    if ( ftStatusB == FT_OK )  {
        cout<<"Status before write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    */
    unsigned char bytes[SEGWAY_USB_MESSAGE_SIZE];

    // convet CAN packet to USB message
    parseCanToUsb( pkt, bytes );

    DWORD bytesWritten;
    FT_STATUS ftStatus = FT_Write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE, &bytesWritten );
    
    //cout<<"result: "<<ftStatus<<" wrote: "<<bytesWritten<<endl;
    
    /*
    // debug
    //DWORD bytesInRxUsb, bytesInTxUsb, usbEvent;
    FT_STATUS ftStatusA = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );    
    if ( ftStatusA == FT_OK )  {
        cout<<"Status after write: rx: "<<bytesInRxUsb<<endl;
        //cout<<"Status on write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    ftStatusA = FT_GetStatus( ftHandle_, &bytesInRxUsb, &bytesInTxUsb, &usbEvent );
    if ( ftStatusA == FT_OK )  {
        cout<<"Status after write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    */
    
    /*
    if ( ftStatus == FT_OK ) {
        //DTRACE<<"wrote "<<bytesWritten<<" bytes"<<endl;
    } else {
        //DTRACE<<"ERROR: failed to write to USB"<<endl;  
    }
    */
    if ( ftStatus == FT_OK ) {
        return UsbIo::OK;
    }
    else {
        return UsbIo::IO_ERROR;
    }
}

/*
 *  Caluate the checksum for the message being sent.  Note that the buffer
 *  is expected to be exactly 18 bytes in length. The checksum byte is not set,
 *  the calling function is expected to do so.
 */
unsigned char
UsbIoFtdi::usbMessageChecksum( unsigned char *msg )
{
    unsigned short checksum;
    unsigned short checksum_hi;
    checksum = 0;

    // don't include the last byte, that where the checksum will go
    for(int i = 0; i < 17; i++)
    {
        checksum += (short)msg[i];
    }

    checksum_hi = (unsigned short)(checksum >> 8);
    checksum &= 0xff;
    checksum += checksum_hi;
    checksum_hi = (unsigned short)(checksum >> 8);
    checksum &= 0xff;
    checksum += checksum_hi;
    checksum = (~checksum + 1) & 0xff;

    return (unsigned char)checksum;
}

// Returns OK if got a packet, NO_DATA if the buffer was empty
UsbIo::UsbIoStatus
UsbIoFtdi::getPacket(CanPacket* pkt)
{
    // check for buffer size, otherwise pop() will seg.fault.
    if ( canBuffer_.empty() ) {
        return UsbIo::NO_DATA;
    }
    
    *pkt = canBuffer_.front();
    canBuffer_.pop();
    //cout<<"UsbIoFtdi::getPacket: got one packet"<<endl;
    return UsbIo::OK;
}

// returns 0 if all is good, -1 on error.
// 'all is good' may mean:
// - woke up, try to read, success
// - woke up, try to read, nothing there
// - timed out.
// possible sources of erros:
// - FTDI errors (FT_SetEventNotification)
// - non-blocking usb2buffer errors (FT_GetQueueStatus, FT_Read )
// - obscure pthread errors (we assert them)
UsbIo::UsbIoStatus
UsbIoFtdi::readFromUsbToBufferBlocking()
{
    DWORD EventMask = FT_EVENT_RXCHAR;
    FT_STATUS ftStatus = FT_SetEventNotification( ftHandle_, EventMask, (PVOID)&eventHandle_ );
    if ( ftStatus != FT_OK ) {
        cerr<<"UsbIoFtdi::readFromUsbToBufferBlocking: failed to set event notification"<<endl;
        return UsbIo::IO_ERROR;
    }

    // we want to timeout from listening to USB after 100ms (10Hz)
    // this is reasonable because, normally the packets comes in at >100Hz.
    int dusec = 100000; // [usec]
    // ugly-ass C time structures
    timeval tval;
    gettimeofday( &tval, NULL );
    if ( (tval.tv_usec + dusec)<1000000 ) {
        tval.tv_usec = tval.tv_usec + dusec;
    }
    else {
        ++tval.tv_sec;
        tval.tv_usec = tval.tv_usec + dusec - 1000000;
    }
    timespec abstime;
    abstime.tv_sec = tval.tv_sec;
    abstime.tv_nsec = tval.tv_usec*1000;

    pthread_mutex_lock(&eventHandle_.eMutex);
    //pthread_cond_wait(&eventHandle_.eCondVar, &eventHandle_.eMutex);
    int ret = pthread_cond_timedwait(&eventHandle_.eCondVar, &eventHandle_.eMutex, &abstime );
    pthread_mutex_unlock(&eventHandle_.eMutex);

    // this should not happen
    assert( (ret==0 || ret==ETIMEDOUT) && "pthread error in UsbIoFtdi::readFromUsbToBufferBlocking" );
        
    if ( ret == 0 ) {
        // now there should be something in the buffer
        return readFromUsbToBufferNonBlocking();
    }
    else if ( ret == ETIMEDOUT ) {
        // this is still ok
        return UsbIo::NO_DATA;
    }
    else {
        // this is one several possible errors
        return UsbIo::OTHER_ERROR;
    }
}

// returns 0 if all is good, -1 on error.
// 'all is good' may mean:
// - try to read, success
// - try to read, nothing there
// possible sources of erros:
// - FTDI errors (FT_GetQueueStatus, FT_Read )
UsbIo::UsbIoStatus
UsbIoFtdi::readFromUsbToBufferNonBlocking()
{
    FT_STATUS   ftStatus;    
    DWORD       bytesInRxUsb;
    DWORD       bytesRead;
        
    // Query status to find out how many bytes are in the receive buffer 
    ftStatus = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );
    if ( ftStatus != FT_OK ) {
        cerr << "ERROR: UsbIoFtdi::readFromUsbToBufferNonBlocking: error in FT_GetQueueStatus" << endl;
        return UsbIo::IO_ERROR;
    }

    // If there's nothing in the receive buffer, then just return
    if ( bytesInRxUsb == 0) {
        return UsbIo::NO_DATA;
    }

    // Resize our buffer if necessary. can this return an error somehow?
    if( (bytesInRxUsb + charBufferBytes_) > charBuffer_.size() ) {
        charBuffer_.resize( bytesInRxUsb + charBufferBytes_ );
    }

    //
    // Read from the USB buffer
    //
    ftStatus = FT_Read( ftHandle_, &charBuffer_[charBufferBytes_], bytesInRxUsb, &bytesRead );

    if ( ftStatus != FT_OK ) {
        cerr << "ERROR: UsbIoFtdi::readFromUsbToBufferNonBlocking: error in FT_Read" << endl;
        return UsbIo::IO_ERROR;
    }

    // Check for read timeout
    // this is probably not a bad error. maybe more bytes have arrived since FT_GetQueueStatus call.
    if( bytesInRxUsb != bytesRead ) {
        cerr << "WARNING: UsbIoFtdi::readFromUsbToBufferNonBlocking: mismatch: bytesInRxUsb = " << bytesInRxUsb << "   bytesRead = " << bytesRead << endl;
        //return -1;
    }

    //cout<<"UsbIoFtdi::readFromUsbToBufferNonBlocking: read "<<bytesRead<<" bytes."<<endl;
    // Adjust the number of bytes that are now in the buffer
    charBufferBytes_ = charBufferBytes_ + bytesRead;
    
    return UsbIo::OK;
}

UsbIo::UsbIoStatus
UsbIoFtdi::readFromBufferToQueue()
{
    int     pos             = 0;        // Buffer position indicator
    bool    finished        = false;
    int     skippedBytes    = 0;
    int     numPackets      = 0;

    // While the buffer contains enough data for at least one message 
    while ( (charBufferBytes_ - pos) >= SEGWAY_USB_MESSAGE_SIZE )
    {
        //cout<<"UsbIoFtdi::readFromBufferToQueue: processing chars into msg #"<<numPackets<<endl;
        bool    lookingForMsgStart  = true;

        // Scan buffer for the start of the next valid message
        while ( lookingForMsgStart )
        {
            // If there aren't enough bytes left to contain a full message, then kick out
            if( (charBufferBytes_ - pos) < SEGWAY_USB_MESSAGE_SIZE )
            {
                //cout<<"UsbIoFtdi::readFromBufferToQueue: not enough char's :"<<(charBufferBytes_ - pos)<<endl;
                finished = true;
                break;
            }

            // Look for start-of-msg character at the start of the candidate message
            if( charBuffer_[pos] == SEGWAY_USB_MSG_START )
            {        
                // Compare calculated checksum with value embedded in message. If
                // they agree, message found. Otherwise, keep looking. 
                if( (unsigned char)usbMessageChecksum(&charBuffer_[pos]) ==
                                    charBuffer_[pos + (SEGWAY_USB_MESSAGE_SIZE - 1)])
                    lookingForMsgStart = false;
                else
                {
                    ++pos;
                    ++skippedBytes;
                }
            }
            else
            {
                ++pos;
                ++skippedBytes;
            }
        } // while (lookingForMsgStart)
        
        // Process message
        if ( !finished )
        {
            CanPacket pkt;
            int ret;

            // Convert from USB message to CAN packet
            ret = parseUsbToCan( &pkt, &charBuffer_[pos] );

            // check that there was some data (there's a lot of garbage here)
            if ( ret == 8 )
            {
                // Store CAN packet in queue
                canBuffer_.push( pkt );
                ++numPackets;
                //printf("Stored packet: pkt: %s\n", pkt.toString());
            }

            // Increment position regardless of parsing success
            pos = pos + SEGWAY_USB_MESSAGE_SIZE;
        }
    } //  while ( (charBufferBytes_ - (pos + 1)) >= SEGWAY_USB_MESSAGE_SIZE )

    // store the number of unread bytes in the member variable 'charBufferBytes_'
    // If there are any residual bytes in the buffer, move them to the beginning
    charBufferBytes_ = charBufferBytes_ - pos;
    assert( charBufferBytes_ >= 0 );

    if( charBufferBytes_ > 0 ) {
        //cout<<"Buffer left with "<<charBufferBytes_<<" bytes."<<endl;
        memcpy( &charBuffer_[0], &charBuffer_[pos], charBufferBytes_ );
    }
    /*
    if ( skippedBytes>0 ) {
        cout<<"Skipped "<<skippedBytes<<" bytes out of "<<charBufferBytes_<<endl;
    } */
    
    // all is good
    return UsbIo::OK;
}

/*
 *  returns: # bytes in msg if a packet is valid and negative on error.
 *
 *  @note It's assumed that the checksum is already checked.
 */
int UsbIoFtdi::parseUsbToCan( CanPacket *pkt, unsigned char *bytes )
{
    int ret;
/*
// this is apparently no longer true
    if (bytes[1] == SEGWAY_USB_HEARTBEAT_MESSAGE) {
        //this message is a HEARTBEAT
        cout<<"got heartbeat"<<endl;
        ret = 0;
    }
    else  
*/
    if (bytes[1]==SEGWAY_USB_STATUS_MESSAGE && bytes[2]==SEGWAY_USB_CHANNEL_A )
    {
        pkt->id = ((bytes[4] << 3) | ((bytes[5] >> 5) & 7)) & 0x0fff;

        if ( (pkt->id>=RMP_CAN_ID_MSG0 && pkt->id<=RMP_CAN_ID_MSG7) 
                || pkt->id==RMP_CAN_ID_STATUS || pkt->id==RMP_CAN_ID_HEARTBEAT )
        {
            for ( int i = 0; i < 8; ++i )
            {
                pkt->msg[i] = bytes[9+i];
            }
            ret = 8;
        }
        else {
            // this is not a status message, no need to queue it
            cout<<"got a status message with unexpected ID : "<<(int)pkt->id<<endl;
            ret = 0;
        }
    }
    else if ( bytes[2]!=SEGWAY_USB_CHANNEL_A )
    {
        // channel B message, ignore
        ret = 0;
    }
    else {
        // we are f*d
        cout<<"got unknown message, first byte : "<<(int)bytes[1]<<endl;
        ret = -1;
    }

    return ret;
}

/*
 *  See Section 3.4 USB Message Format of the Segway RMP manual.
 */
int UsbIoFtdi::parseCanToUsb( CanPacket *pkt, unsigned char *bytes )
{
    bytes[0] = SEGWAY_USB_MSG_START;   //SOH (alexm: start of header?)
    bytes[1] = SEGWAY_USB_STATUS_MESSAGE;
    bytes[2] = 0;   // CAN A
    bytes[3] = 0;   // always 0
    bytes[4] = 0;   // always 0
    bytes[5] = 0;   // always 0
    bytes[6] = pkt->id >> 8;
    bytes[7] = pkt->id ;
    bytes[8] = 0;   // always 0

    for ( int i = 0; i < 8; ++i )
    {
        bytes[9+i] = pkt->msg[i];
    }
    
    // do check sum
    bytes[17] = usbMessageChecksum( bytes );

    return 0;
}
