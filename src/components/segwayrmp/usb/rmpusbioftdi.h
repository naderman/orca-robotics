/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_USB_IO_FTDI_H
#define ORCA2_SEGWAY_RMP_USB_IO_FTDI_H

#include <queue>
#include <ftd2xx.h>
#include <usbftdi/usbftdi.h>
#include "rmpusbio.h"

namespace segwayrmp
{

//
//
//
class RmpUsbIoFtdi : public RmpUsbIo
{
public:
    RmpUsbIoFtdi( int debugLevel=0 );
    virtual ~RmpUsbIoFtdi();
    
    virtual void init();

    // Tries to reset the device without shutting it down completely.
    virtual void reset();
    
    virtual void shutdown();
    
    // Returns OK if copied a packet, NO_DATA if not
    virtual RmpUsbIoStatus readPacket( CanPacket* pkt );
    
    virtual void writePacket( CanPacket* pkt );
    
private:

    // STORAGE
    
    // a char buffer to read USB stream into
    std::vector<unsigned char> charBuffer_;
    // have to remember how many char are left in the buffer
    unsigned int charBufferBytes_;
    // a buffer to store parsed CAN messages
    std::queue<CanPacket> canBuffer_;

    // HARDWDARE
    
    // handle to the USB device inside our Segway
    usbftdi::UsbFtdi *usbFtdi_;
    
    // Don't call this if the buffer's empty.
    void getPacketFromCanBuffer( CanPacket* pkt );
    
    RmpUsbIoStatus readPacketNonBlocking( CanPacket* pkt );
    RmpUsbIoStatus readPacketBlocking( CanPacket* pkt );
    // Not used but keep for reference
    //RmpUsbIoStatus readPacketPolling( CanPacket* pkt );
    
    RmpUsbIoStatus readFromUsbToBufferNonBlocking();
    RmpUsbIoStatus readFromUsbToBufferBlocking();
    void readFromBufferToQueue();
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );    

    int debugLevel_;
};

} // namespace

#endif
