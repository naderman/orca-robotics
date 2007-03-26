/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <IceUtil/IceUtil.h>

#include "rmpusbioftdi.h"
#include <rmpdriver/rmpdataframe.h>
#include <rmpdriver/rmpdefs.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/usb/usbftdi/usbftdi.h>
#include <rmpexception.h>

// USB magic numbers
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

namespace segwayrmp {

RmpUsbIoFtdi::RmpUsbIoFtdi()
    : isEnabled_(false),
      charBuffer_(128),
      charBufferBytes_(0),
      usbFtdi_(NULL)
{
}

RmpUsbIoFtdi::~RmpUsbIoFtdi()
{
    disable();
}

void
RmpUsbIoFtdi::enable( int debugLevel )
{
    debugLevel_ = debugLevel;
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbioftdi.cpp): enable()" << endl;

    if ( isEnabled_ ) return;

    try {
        usbFtdi_ = new usbftdi::UsbFtdi( SEGWAY_USB_VENDOR_ID,
                                         SEGWAY_USB_PRODUCT_ID,
                                         SEGWAY_USB_DESCRIPTION,
                                         debugLevel_ );
    }
    catch ( usbftdi::Exception &e )
    {
        stringstream ss;
        ss << "RmpUsbIoFtdi::constructor(): Error: "<<e.what();
        throw RmpException( ss.str() );
    }

    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbioftdi.cpp): enable() succeeded" << endl;    

    isEnabled_ = true;
}

void
RmpUsbIoFtdi::disable()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbioftdi.cpp): disable()" << endl;
    if ( !isEnabled_ ) return;
    
    if ( usbFtdi_ ) delete usbFtdi_;

    isEnabled_ = false;
}

// void
// RmpUsbIoFtdi::reset()
// {
//     if ( debugLevel_ > 0 )
//         cout<<"TRACE(rmpusbioftdi.cpp): reset()" << endl;

//     assert( usbFtdi_ != NULL );

//     //
//     // Nuclear reset...
//     //
//     try {
//         if (usbFtdi_ != NULL)
//         {
//             delete usbFtdi_;
//             usbFtdi_ = NULL;
//         }        
//         init();
//     }
//     catch ( usbftdi::Exception &e )
//     {
//         stringstream ss;
//         ss << "RmpUsbIoFtdi::reset() Error resetting USB device: " << e.what();
//         throw Exception( ss.str() );
//     }
// }
 
// void
// RmpUsbIoFtdi::shutdown()
// {
//     if ( debugLevel_ > 0 )
//         cout<<"TRACE(rmpusbioftdi.cpp): shutdown()" << endl;

//     if (usbFtdi_ != NULL) 
//     {
//         cout<<"TRACE(rmpusbioftdi.cpp): deleting usbFtdi_" << endl;
//         delete usbFtdi_;
//         usbFtdi_ = NULL;
//         cout<<"TRACE(rmpusbioftdi.cpp): deleted." << endl;
//     }
// }

RmpIo::RmpIoStatus
RmpUsbIoFtdi::readPacket(CanPacket* pkt)
{
    assert( usbFtdi_ != 0 );

    try {
        RmpIo::RmpIoStatus status;

        // First try non-blocking (maybe there's a packet there already)
        status = readPacketNonBlocking( pkt );
        
        if ( status == NO_DATA )
            return readPacketBlocking( pkt );
        else
            return status;
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "RmpUsbIoFtdi::readPacket(): Error: " << e.what();
        throw RmpException(ss.str());
    }
}

RmpIo::RmpIoStatus
RmpUsbIoFtdi::readPacketNonBlocking( CanPacket* pkt )
{
    RmpIo::RmpIoStatus status;
    
    // if there's already a packet in CAN packet buffer, just return it
    if ( !canBuffer_.empty() )
    {
        getPacketFromCanBuffer( pkt );
        return OK;
    }

    // there's no packet ready to be returned.
    // Try to read from the USB buffer into our own char buffer
    status = readFromUsbToBufferNonBlocking();
    
    if ( status == OK )
    {
        // Try to parse buffer contents into CAN packets
        readFromBufferToQueue();
    }
    
    // try to return a packet again
    if ( !canBuffer_.empty() )
    {
        getPacketFromCanBuffer( pkt );
        return OK;
    }

    // did not get a packet, but it's not an error
    return NO_DATA;
}

RmpIo::RmpIoStatus
RmpUsbIoFtdi::readPacketBlocking( CanPacket* pkt )
{
    RmpIo::RmpIoStatus status;
    
    // arbitrary parameter
    int blockCount = 5;
    
    while ( blockCount )
    {
        --blockCount;

        // this blocking call uses a timed conditional variable, so it shouldn't lock up forever
        status = readFromUsbToBufferBlocking();
        
        if ( status == OK )
        {
            // Try to parse buffer contents into CAN packets
            readFromBufferToQueue();
        
            // try to return a packet
            if ( !canBuffer_.empty() )
            {
                getPacketFromCanBuffer( pkt );
                return OK;
            }
        }
    }
    
    // did not get a packet, but it's not an error
    return NO_DATA;
}

// // this function is not currently used.
// // Use readFromUsbToBufferBlocking(), it is more efficient.
// RmpUsbIo::RmpUsbStatus
// RmpUsbIoFtdi::readPacketPolling( CanPacket* pkt )
// {
//     RmpUsbIo::RmpUsbStatus status;

//     if ( !canBuffer_.empty() )
//     {
//         getPacketFromCanBuffer( pkt );
//         return RmpUsbIo::OK;
//     }

//     // made up variables
//     const int pollingIntervalUsec = 20000;
//     int pollCount = 10;

//     while ( pollCount )
//     {
//         --pollCount;

//         // Read from the USB buffer into our own buffer
//         unsigned int bytesInBuffer = charBufferBytes_;
        
//         status = readFromUsbToBufferNonBlocking();
        
//         // if the number of bytes in the buffer hasn't changed, try again at 50Hz
//         if ( charBufferBytes_ == bytesInBuffer ) {
//             IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(pollingIntervalUsec));
//             continue;
//         }
    
//         // Try to parse buffer contents into CAN packets
//         status = readFromBufferToQueue();
        
//         // try to return a packet
//         if ( !canBuffer_.empty() )
//         {
//             getPacketFromCanBuffer( pkt );
//             return RmpUsbIo::OK;
//         }
//     }

//     // did not get a packet, but it's not an error
//     return RmpUsbIo::NO_DATA;
// }
  
void
RmpUsbIoFtdi::writePacket( CanPacket *pkt )
{
    assert( usbFtdi_ != 0 );

    unsigned char bytes[SEGWAY_USB_MESSAGE_SIZE];

    // convet CAN packet to USB message
    parseCanToUsb( pkt, bytes );

    try {
        usbFtdi_->write( bytes, SEGWAY_USB_MESSAGE_SIZE );
    }
    catch ( usbftdi::Exception &e )
    {
        stringstream ss;
        ss << "RmpUsbIoFtdi::writePacket(): Error: " << e.what();

        //
        // OK, so the write failed.  Is the Segway still writing to us?
        //
        try { 
            for ( int i=0; i < 3; i++ )
                usbFtdi_->waitForData();
            ss << endl << " --> The Segway is still writing to us though.  Seems like we're in that fucked-up state.";
        }
        catch ( usbftdi::Exception &e )
        {
            ss << endl << " --> The Segway is not writing to us.  Looks like it's powered off.";
        }

        throw RmpException( ss.str() );
    }    
}

/*
 *  Caluate the checksum for the message being sent.  Note that the buffer
 *  is expected to be exactly 18 bytes in length. The checksum byte is not set,
 *  the calling function is expected to do so.
 */
unsigned char
RmpUsbIoFtdi::usbMessageChecksum( unsigned char *msg )
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

// // Returns OK if got a packet, NO_DATA if the buffer was empty
// RmpUsbIo::RmpUsbStatus
// RmpUsbIoFtdi::getPacket(CanPacket* pkt)
// {
//     // check for buffer size, otherwise pop() will seg.fault.
//     if ( canBuffer_.empty() ) {
//         return RmpUsbIo::NO_DATA;
//     }
    
//     *pkt = canBuffer_.front();
//     canBuffer_.pop();
//     //cout<<"RmpUsbIoFtdi::getPacket: got one packet"<<endl;
//     return RmpUsbIo::OK;
// }

void
RmpUsbIoFtdi::getPacketFromCanBuffer(CanPacket *pkt)
{
    assert( !canBuffer_.empty() );
    *pkt = canBuffer_.front();
    canBuffer_.pop();
}

// returns 0 if all is good
// 'all is good' may mean:
// - woke up, try to read, success
// - woke up, try to read, nothing there
// - timed out.
// possible sources of erros:
// - FTDI errors (FT_SetEventNotification)
// - non-blocking usb2buffer errors (FT_GetQueueStatus, FT_Read )
// - obscure pthread errors (we assert them)
RmpIo::RmpIoStatus
RmpUsbIoFtdi::readFromUsbToBufferBlocking()
{
    int ret;
    try {
        ret = usbFtdi_->waitForData();
    }
    catch ( usbftdi::Exception &e )
    {
        stringstream ss;
        ss << "RmpUsbIoFtdi::readFromUsbToBufferBlocking(): Error: " << e.what();
        throw RmpException( ss.str() );
    }
    if ( ret == usbftdi::USBFTDI_OK )
    {
        return readFromUsbToBufferNonBlocking();
    }
    else if ( ret == usbftdi::USBFTDI_TIMEOUT )
    {
        // this is still ok
        return NO_DATA;
    }
    else
    {
        throw RmpException( "RmpUsbIoFtdi::readFromUsbToBufferBlocking(): Unknown return type" );
    }
}

// returns 0 if all is good
// 'all is good' may mean:
// - try to read, success
// - try to read, nothing there
// possible sources of erros:
// - FTDI errors (FT_GetQueueStatus, FT_Read )
RmpIo::RmpIoStatus
RmpUsbIoFtdi::readFromUsbToBufferNonBlocking()
{
    try {

        int bytesInRxUsb = usbFtdi_->bytesInRxQueue();

        if ( bytesInRxUsb == 0) {
            return NO_DATA;
        }

        // Resize our buffer if necessary.
        if( (bytesInRxUsb + charBufferBytes_) > charBuffer_.size() )
            charBuffer_.resize( bytesInRxUsb + charBufferBytes_ );

        // Read the bytes that we know are there
        int numBytesRead = usbFtdi_->readNonBlocking( &charBuffer_[charBufferBytes_], bytesInRxUsb );

        // Adjust the number of bytes that are now in the buffer
        charBufferBytes_ = charBufferBytes_ + numBytesRead;

        return OK;
    }
    catch ( usbftdi::Exception &e )
    {
        stringstream ss;
        ss << "RmpUsbIoFtdi::readFromUsbToBufferNonBlocking(): Error: " << e.what();
        throw RmpException(ss.str());
    }
}

void
RmpUsbIoFtdi::readFromBufferToQueue()
{
    int     pos             = 0;        // Buffer position indicator
    bool    finished        = false;
    int     skippedBytes    = 0;
    int     numPackets      = 0;

    // While the buffer contains enough data for at least one message 
    while ( (charBufferBytes_ - pos) >= SEGWAY_USB_MESSAGE_SIZE )
    {
        //cout<<"RmpUsbIoFtdi::readFromBufferToQueue: processing chars into msg #"<<numPackets<<endl;
        bool    lookingForMsgStart  = true;

        // Scan buffer for the start of the next valid message
        while ( lookingForMsgStart )
        {
            // If there aren't enough bytes left to contain a full message, then kick out
            if( (charBufferBytes_ - pos) < SEGWAY_USB_MESSAGE_SIZE )
            {
                //cout<<"RmpUsbIoFtdi::readFromBufferToQueue: not enough char's :"<<(charBufferBytes_ - pos)<<endl;
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
}

/*
 *  returns: # bytes in msg if a packet is valid and negative on error.
 *
 *  @note It's assumed that the checksum is already checked.
 */
int RmpUsbIoFtdi::parseUsbToCan( CanPacket *pkt, unsigned char *bytes )
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
int RmpUsbIoFtdi::parseCanToUsb( CanPacket *pkt, unsigned char *bytes )
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

}
