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

#include "usbio.h"

namespace segwayrmp
{

class UsbIoFtdi : public UsbIo
{
public:
    UsbIoFtdi();
    virtual ~UsbIoFtdi();
    
    // Returns: 0 on success, non-zero otherwise.
    virtual UsbIoStatus init();

    // Tries to reset the device whitout shutting it down completely.
    virtual UsbIoStatus reset();
    
    // Returns: 0 on success, non-zero otherwise.
    virtual UsbIoStatus shutdown();
    
    // Returns OK if copied a packet, NO_DATA if not, or a negative ERROR code.
    virtual UsbIoStatus readPacket( CanPacket* pkt );
    
    // Returns OK on success, or a negative ERROR code otherwise.
    virtual UsbIoStatus writePacket( CanPacket* pkt );
    
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
    FT_HANDLE ftHandle_;

    // event handle we'll use to block on data arrivale
    EVENT_HANDLE eventHandle_;
    
    // LOW-LEVEL READ/WRITE FUNCTIONS
    // Returns 0 if got a packet from the CAN buffer, 1 if the buffer was empty
    UsbIoStatus getPacket( CanPacket* pkt );
    
    // Returns 0 if copied a packet, positive if not, negative on error.
    UsbIoStatus readPacketNonBlocking( CanPacket* pkt );
    // Returns 0 if copied a packet, positive if not, negative on error.
    UsbIoStatus readPacketBlocking( CanPacket* pkt );
    // Not used but keep for reference
    UsbIoStatus readPacketPolling( CanPacket* pkt );
    
    // returns 0 if all is good, -1 on error.
    UsbIoStatus readFromUsbToBufferNonBlocking();
    // returns 0 if all is good, -1 on error.
    UsbIoStatus readFromUsbToBufferBlocking();
    
    // returns 0 if all is good, -1 on error. non-blocking.
    UsbIoStatus readFromBufferToQueue();
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );    
};

} // namespace

#endif
