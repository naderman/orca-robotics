/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_USB_IO_FTDI_H
#define ORCA2_SEGWAY_RMP_USB_IO_FTDI_H

#include <queue>
#include <ftd2xx.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>

namespace usbftdi {
    class UsbFtdi;
}

namespace segwayrmp
{

//
//
//
class RmpUsbIoFtdi : public RmpIo
{
public:

    RmpUsbIoFtdi();
    ~RmpUsbIoFtdi();

    // Initiate comms with the RMP.
    virtual void enable( int debugLevel );

    // Release all resources.
    virtual void disable();
    
    // Returns OK if copied a packet, NO_DATA if not
    RmpIo::RmpIoStatus readPacket( CanPacket* pkt );
    
    void writePacket( CanPacket* pkt );
    
private:

    // STORAGE
    
    bool isEnabled_;

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
    
    RmpIo::RmpIoStatus readPacketNonBlocking( CanPacket* pkt );
    RmpIo::RmpIoStatus readPacketBlocking( CanPacket* pkt );
    // Not used but keep for reference
    //RmpUsbIoStatus readPacketPolling( CanPacket* pkt );
    
    RmpIo::RmpIoStatus readFromUsbToBufferNonBlocking();
    RmpIo::RmpIoStatus readFromUsbToBufferBlocking();
    void readFromBufferToQueue();
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );    

    int debugLevel_;
};

} // namespace

#endif
