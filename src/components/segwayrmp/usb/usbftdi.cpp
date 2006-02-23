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

#include "usbftdi.h"

#include "rmpusbdataframe.h"

#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <IceUtil/IceUtil.h>

// magic numbers
#define SEGWAY_USB_VENDOR_ID                    0x0403
#define SEGWAY_USB_PRODUCT_ID                   0xE729
#define SEGWAY_USB_MESSAGE_SIZE                 18
#define SEGWAY_USB_MSG_START                    0xF0
#define SEGWAY_USB_HEARTBEAT_MESSAGE            0x0
#define SEGWAY_USB_STATUS_MESSAGE               0x55
#define SEGWAY_USB_CHANNEL_A                    0xAA
#define SEGWAY_USB_CHANNEL_B                    0xBB

using namespace std;


UsbIoFtdi::UsbIoFtdi()
{
    // Set initial buffer size
    charBuffer_.resize(128);

    residualBytes_ = 0;
    
    pthread_mutex_init(&eventHandle_.eMutex, NULL);
    pthread_cond_init(&eventHandle_.eCondVar, NULL);
}

UsbIoFtdi::~UsbIoFtdi()
{
}

/*!
 * Initializes the USB device.
 *
 * returns: 0 on success, negative on error
 */
int UsbIoFtdi::init()
{
    // custom device settings
    DWORD iVID = SEGWAY_USB_VENDOR_ID;
    DWORD iPID = SEGWAY_USB_PRODUCT_ID;
    // without this the library will not find the segway
    FT_SetVIDPID(iVID, iPID);   // use our VID and PID;
    
    DWORD num_devices;
    FT_STATUS ftStatus = FT_ListDevices( &num_devices, NULL, FT_LIST_NUMBER_ONLY );
    if ( ftStatus != FT_OK )
    {
        cout<<"couldn't get a list of devices"<<endl;
        return -1;
    }
    
    cout<<"found "<<(int)num_devices<<" devices"<<endl;
    if (num_devices <= 0) {
        return -1;
    }
    
    ftStatus = FT_Open( 0, &ftHandle_ );
    if ( ftStatus != FT_OK) {
        cout<<"FT_Open failed ("<<ftStatus<<")"<<endl;
        return ftStatus;
    }
    cout<<"FT_Open OK"<<endl;

    ftStatus = FT_SetBaudRate( ftHandle_, 460800 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetBaudRate failed ("<<ftStatus<<")"<<endl;
        FT_Close( ftHandle_ );        //close open device
        return ftStatus;
    }   
    cout<<"FT_SetBaudRate OK"<<endl;

    //set the latency timer to 2ms (valid range is from 2 to 255 ms)
    ftStatus = FT_SetLatencyTimer( ftHandle_, 2 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetLatencyTimer failed ("<<ftStatus<<")"<<endl;
        //latency not set - but we won't know   return -82;
        return ftStatus;
    }    
    cout<<"FT_SetLatencyTimer OK"<<endl;

    ftStatus = this->resetDevice();


    // debug
    /*
    CanPacket ppp;
    for ( int i=0; i<10; ++i )
    {
        ReadPacket( &ppp, 0 );
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(50));
    }
    */   

  return ftStatus;
}

int UsbIoFtdi::resetDevice()
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

    return ftStatus;
}
  
/*!
 * Closes the USB device
 *
 * returns: 0 on success, negative otherwise
 */
int UsbIoFtdi::shutdown()
{
    return FT_Close( ftHandle_ );
}

int UsbIoFtdi::readPacket(CanPacket* pkt)
{
    return readPacketBlocking( pkt );
    //return readPacketPolling( pkt );
}

int UsbIoFtdi::readPacketBlocking( CanPacket* pkt )
{
    if ( !getPacket( pkt ) ) {
        return 0;
    }

    // Read from the USB buffer into our own buffer
    int bytesInBuffer = readFromUsbToBuffer();
    // Process the buffer contents into CAN packets
    if( bytesInBuffer > 0 ) {
        //cout<<"read "<<bytesInBuffer<<" bytes without blocking"<<endl;
        // Process the buffer contents into CAN packets
        int numPackets = readFromBufferToQueue( bytesInBuffer );
        if ( numPackets > 0 ) {
            return getPacket( pkt );
        }
        else {
            cerr<<"sbIoFtdi::readPacketBlocking: couldn't read any packets from non-empty buffer"<<endl;
            return 1;
        }
    }
    
    // nothing in USB buffer right now, let's wait
    int numberOfBlocks = 0;
    const int maxNumberOfBlocks = 5;
    while ( numberOfBlocks < maxNumberOfBlocks ) {
        ++numberOfBlocks;

        // this blocking call uses a timed conditional variable, so it shouldn't lock up forever
        bytesInBuffer = readFromUsbToBufferBlocking();

        // Process the buffer contents into CAN packets
        if( bytesInBuffer > 0 ) {
            // Process the buffer contents into CAN packets
            int numPackets = readFromBufferToQueue( bytesInBuffer );
            if ( numPackets > 0 ) {
                return getPacket( pkt );
            }
            else {
                //cout<<"not enought data, re-blocking"<<endl;
            }
        }
    }
    
    return 1;
}

int UsbIoFtdi::readPacketPolling( CanPacket* pkt )
{
    if ( !getPacket( pkt ) ) {
        return 0;
    }

    // made up variables
    const int maxNumberOfPolls = 10;
    const int pollingIntervalUsec = 20000;

    int numberOfPolls = 0;

    while ( numberOfPolls<maxNumberOfPolls )
    {
        ++numberOfPolls;

        if ( readFromUsbToBuffer()==0 ) {
            // try again at 50Hz
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(pollingIntervalUsec));
            continue;
        }
    
        // Read from the USB buffer into our own buffer
        int bytesInBuffer = readFromUsbToBuffer();
    
        // Process the buffer contents into CAN packets
        if( bytesInBuffer > 0 )
        {
            // Process the buffer contents into CAN packets
            int numPackets = readFromBufferToQueue( bytesInBuffer );
            if ( numPackets > 0 ) {
                return getPacket( pkt );
            }
            else {
                //cout<<"not enough data, re-reading"<<endl;
            }
        }    
    }

    return 0;
}
  
/*!
 * Writes the given packet to USB.
 *
 * returns: 0 on success, negative error code otherwise
 */
int UsbIoFtdi::writePacket( CanPacket *pkt )
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
    return ftStatus;
}

/*!
 *  Caluate the checksum for the message being sent.  Note that the buffer
 *  is expected to be exactly 18 bytes in length. The checksum byte is not set,
 *  the calling function is expected to do so.
 */
unsigned char UsbIoFtdi::usbMessageChecksum( unsigned char *msg )
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

int UsbIoFtdi::getPacket(CanPacket* pkt)
{

    if ( canBuffer_.size() > 0 )
    {
        *pkt = canBuffer_.front();
        canBuffer_.pop();
        //cout<<"UsbIoFtdi::getPacket: read one packet"<<endl;
        return 0;
    }
    else {
        return 1;
    }
}

// returns number of bytes read
int UsbIoFtdi::readFromUsbToBufferBlocking()
{
    DWORD EventMask = FT_EVENT_RXCHAR;
    FT_STATUS ftStatus = FT_SetEventNotification( ftHandle_, EventMask, (PVOID)&eventHandle_ );
    if ( ftStatus != FT_OK ) {
        cerr<<"UsbIoFtdi::readFromUsbToBufferBlocking: failed to set event notification"<<endl;
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

    if ( ret == ETIMEDOUT ) {
        return 0;
    }
    else {
        // now there should be something in the buffer
        return readFromUsbToBuffer();
    }
}

int UsbIoFtdi::readFromUsbToBuffer()
{
    FT_STATUS   ftStatus;    
    DWORD       bytesInRxUsb;
    DWORD       bytesRead;
        
    // Query status to find out how many bytes are in the receive buffer 
    ftStatus = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );
    if ( ftStatus != FT_OK ) {
        //! @todo is this right?
        bytesInRxUsb = 0;
    }

    // If there's nothing in the receive buffer, then just return
    if ( bytesInRxUsb == 0) {
        return 0;
    }

    // Resize our buffer if necessary
    if( (bytesInRxUsb + residualBytes_) > charBuffer_.size() )
        charBuffer_.resize( bytesInRxUsb + residualBytes_ );

    // Read from the USB buffer
    ftStatus = FT_Read( ftHandle_, &charBuffer_[residualBytes_], bytesInRxUsb, &bytesRead );

    if ( ftStatus != FT_OK ) {
        cerr << "read error" << endl;
        return 0;
    }

    // Check for read timeout
    if( bytesInRxUsb != bytesRead ) {
        cerr << "UsbIoFtdi::readFromUsbToBuffer: mismatch: bytesInRxUsb = " << bytesInRxUsb << "   bytesRead = " << bytesRead << endl;
    }

    //cout<<"UsbIoFtdi::readFromUsbToBuffer: read "<<bytesRead<<" bytes."<<endl;
    // Return the number of bytes that are now in the buffer
    return residualBytes_ + bytesRead;
}

int UsbIoFtdi::readFromBufferToQueue( int bytesInBuffer )
{
    int     pos             = 0;        // Buffer position indicator
    bool    finished            = false;
    int     skippedBytes    = 0;
    int     numPackets      = 0;

    // While the buffer contains enough data for at least one message 
    while ( (bytesInBuffer - pos) >= SEGWAY_USB_MESSAGE_SIZE )
    {
        //cout<<"UsbIoFtdi::readFromBufferToQueue: processing chars into msg #"<<numPackets<<endl;
        bool    lookingForMsgStart  = true;

        // Scan buffer for the start of the next valid message
        while ( lookingForMsgStart )
        {
            // If there aren't enough bytes left to contain a full message, then kick out
            if( (bytesInBuffer - pos) < SEGWAY_USB_MESSAGE_SIZE )
            {
                //cout<<"UsbIoFtdi::readFromBufferToQueue: not enough char's :"<<(bytesInBuffer - pos)<<endl;
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
    } //  while ( (bytesInBuffer - (pos + 1)) >= SEGWAY_USB_MESSAGE_SIZE )

    // If there are any residual bytes in the buffer, move them to the beginning
    residualBytes_ = bytesInBuffer - pos;
    assert( residualBytes_ >= 0 );

    if( residualBytes_ > 0 ) {
        //cout<<"Buffer left with "<<residualBytes_<<" bytes."<<endl;
        memcpy( &charBuffer_[0], &charBuffer_[pos], residualBytes_ );
    }
    /*
    if ( skippedBytes>0 ) {
        cout<<"Skipped "<<skippedBytes<<" bytes out of "<<bytesInBuffer<<endl;
    } */
    return numPackets;
}

/*!
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

/*!
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
