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
#include "canpacket.h"

namespace segwayrmp
{

//
// Exceptions thrown around in segwayrmp.
//
class Exception : public std::exception
{
public:

    Exception(const char *message)
        : message_(message) {}
    Exception(const std::string &message)
        : message_(message) {}

    virtual ~Exception() throw() {}

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    std::string  message_;
};

//
//
//
class RmpUsbIoFtdi
{
public:

    enum RmpUsbStatus
    {
        OK              = 0,
        NO_DATA         = 1,
    };

    RmpUsbIoFtdi( int debugLevel=0 );
    ~RmpUsbIoFtdi();
    
    // Returns OK if copied a packet, NO_DATA if not
    RmpUsbStatus readPacket( CanPacket* pkt );
    
    void writePacket( CanPacket* pkt );
    
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
    
    RmpUsbStatus readPacketNonBlocking( CanPacket* pkt );
    RmpUsbStatus readPacketBlocking( CanPacket* pkt );
    // Not used but keep for reference
    //RmpUsbIoStatus readPacketPolling( CanPacket* pkt );
    
    RmpUsbStatus readFromUsbToBufferNonBlocking();
    RmpUsbStatus readFromUsbToBufferBlocking();
    void readFromBufferToQueue();
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );    

    int debugLevel_;
};

} // namespace

#endif
