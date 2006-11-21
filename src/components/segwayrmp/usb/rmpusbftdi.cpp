/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <IceUtil/IceUtil.h>

#include "rmpusbftdi.h"

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


RmpUsbIoFtdi::RmpUsbIoFtdi( int debugLevel )
    : usbFtdi_(NULL),
      debugLevel_(debugLevel)
{
    // Set initial buffer size and number of bytes stored
    charBuffer_.resize(128);
    charBufferBytes_ = 0;
}

RmpUsbIoFtdi::~RmpUsbIoFtdi()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbftdi.cpp): destructor()" << endl;

    // usbFtdi_ should be destroyed in shutdown().
    //if ( usbFtdi_ ) delete usbFtdi_;
}

RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::init()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbftdi.cpp): init()" << endl;

    try {
        usbFtdi_ = new usbftdi::UsbFtdi( SEGWAY_USB_VENDOR_ID,
                                         SEGWAY_USB_PRODUCT_ID,
                                         SEGWAY_USB_DESCRIPTION,
                                         debugLevel_ );
    }
    catch ( usbftdi::Exception &e )
    {
        cout << "ERROR(rmpusbftdi.cpp): Error initialising USB device: " << e.what() << endl;
        return RmpUsbIo::IO_ERROR;
    }

    return RmpUsbIo::OK;
}

RmpUsbIo::RmpUsbIoStatus RmpUsbIoFtdi::reset()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbftdi.cpp): reset()" << endl;

    assert( usbFtdi_ != NULL );

    //
    // Nuclear reset...
    //
    try {
        if (usbFtdi_ != NULL) delete( usbFtdi_ );
        init();
    }
    catch ( usbftdi::Exception &e )
    {
        cout << "ERROR(rmpusbftdi.cpp): Error resetting USB device: " << e.what() << endl;
        return RmpUsbIo::IO_ERROR;
    }

    return RmpUsbIo::OK;
}
 
RmpUsbIo::RmpUsbIoStatus RmpUsbIoFtdi::shutdown()
{
    if ( debugLevel_ > 0 )
        cout<<"TRACE(rmpusbftdi.cpp): shutdown()" << endl;
    assert( usbFtdi_ != NULL );

    if (usbFtdi_ != NULL) delete usbFtdi_;
    return RmpUsbIo::OK;
}

RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readPacket(CanPacket* pkt)
{
    assert( usbFtdi_ != 0 );

    RmpUsbIo::RmpUsbIoStatus status;

    // First try non-blocking (maybe there's a packet there already)
    status = readPacketNonBlocking( pkt );
 
    if ( status == RmpUsbIo::NO_DATA )
        return readPacketBlocking( pkt );
    else
        return status;
}

RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readPacketNonBlocking( CanPacket* pkt )
{
    RmpUsbIo::RmpUsbIoStatus status;
    
    // if there's already a packet in CAN packet buffer, just return it
    if ( !canBuffer_.empty() )
    {
        getPacketFromCanBuffer( pkt );
        return RmpUsbIo::OK;
    }

    // there's no packet ready to be returned.
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
    if ( !canBuffer_.empty() )
    {
        getPacketFromCanBuffer( pkt );
        return RmpUsbIo::OK;
    }

    // did not get a packet, but it's not an error
    return RmpUsbIo::NO_DATA;
}

RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readPacketBlocking( CanPacket* pkt )
{
    RmpUsbIo::RmpUsbIoStatus status;
    
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
        if ( !canBuffer_.empty() )
        {
            getPacketFromCanBuffer( pkt );
            return RmpUsbIo::OK;
        }
    }
    
    // did not get a packet, but it's not an error
    return RmpUsbIo::NO_DATA;
}

// this function is not currently used.
// Use readFromUsbToBufferBlocking(), it is more efficient.
RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readPacketPolling( CanPacket* pkt )
{
    RmpUsbIo::RmpUsbIoStatus status;

    if ( !canBuffer_.empty() )
    {
        getPacketFromCanBuffer( pkt );
        return RmpUsbIo::OK;
    }

    // made up variables
    const int pollingIntervalUsec = 20000;
    int pollCount = 10;

    while ( pollCount )
    {
        --pollCount;

        // Read from the USB buffer into our own buffer
        unsigned int bytesInBuffer = charBufferBytes_;
        
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
        if ( !canBuffer_.empty() )
        {
            getPacketFromCanBuffer( pkt );
            return RmpUsbIo::OK;
        }
    }

    // did not get a packet, but it's not an error
    return RmpUsbIo::NO_DATA;
}
  
/*
 * Writes the given packet to USB.
 *
 * returns: 0 on success, negative error code otherwise
 */
RmpUsbIo::RmpUsbIoStatus
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
        cout << "ERROR(rmpusbftdi.cpp): Error during write: " << e.what() << endl;
        return RmpUsbIo::IO_ERROR;
    }    
    return RmpUsbIo::OK;
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
// RmpUsbIo::RmpUsbIoStatus
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

// returns 0 if all is good, -1 on error.
// 'all is good' may mean:
// - woke up, try to read, success
// - woke up, try to read, nothing there
// - timed out.
// possible sources of erros:
// - FTDI errors (FT_SetEventNotification)
// - non-blocking usb2buffer errors (FT_GetQueueStatus, FT_Read )
// - obscure pthread errors (we assert them)
RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readFromUsbToBufferBlocking()
{
    int ret;
    try {
        ret = usbFtdi_->waitForData();
    }
    catch ( usbftdi::Exception &e )
    {
        cout << "ERROR(rmpusbftdi.cpp): readFromUsbToBufferBlocking: error: " << e.what() << endl;
        return RmpUsbIo::IO_ERROR;
    }
    if ( ret == usbftdi::USBFTDI_OK )
    {
        return readFromUsbToBufferNonBlocking();
    }
    else if ( ret == usbftdi::USBFTDI_OK )
    {
        // this is still ok
        return RmpUsbIo::NO_DATA;
    }
    else
    {
        assert( false && "Unknown return type" );
        return RmpUsbIo::OTHER_ERROR;
    }
}

// returns 0 if all is good, -1 on error.
// 'all is good' may mean:
// - try to read, success
// - try to read, nothing there
// possible sources of erros:
// - FTDI errors (FT_GetQueueStatus, FT_Read )
RmpUsbIo::RmpUsbIoStatus
RmpUsbIoFtdi::readFromUsbToBufferNonBlocking()
{
    try {

        int bytesInRxUsb = usbFtdi_->bytesInRxQueue();

        if ( bytesInRxUsb == 0) {
            return RmpUsbIo::NO_DATA;
        }

        // Resize our buffer if necessary.
        if( (bytesInRxUsb + charBufferBytes_) > charBuffer_.size() )
            charBuffer_.resize( bytesInRxUsb + charBufferBytes_ );

        // Read the bytes that we know are there
        int numBytesRead = usbFtdi_->readNonBlocking( &charBuffer_[charBufferBytes_], bytesInRxUsb );

        // Adjust the number of bytes that are now in the buffer
        charBufferBytes_ = charBufferBytes_ + numBytesRead;

        return RmpUsbIo::OK;
    }
    catch ( usbftdi::Exception &e )
    {
        cout << "ERROR(rmpusbftdi.cpp): readFromUsbToBufferNonBlocking(): error: " << e.what() << endl;
        return RmpUsbIo::IO_ERROR;
    }
}

RmpUsbIo::RmpUsbIoStatus
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
    
    // all is good
    return RmpUsbIo::OK;
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
