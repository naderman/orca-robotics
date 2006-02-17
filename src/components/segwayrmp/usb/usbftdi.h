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

#ifndef ORCA2_SEGWAY_RMP_USB_IO_FTDI_H
#define ORCA2_SEGWAY_RMP_USB_IO_FTDI_H

#include <queue>

#include "usbio.h"

#include <ftd2xx.h>

class UsbIoFtdi : public UsbIo
{
public:
    UsbIoFtdi();
    virtual ~UsbIoFtdi();
    
    virtual int init();
    virtual int shutdown();

    virtual int readPacket(CanPacket* pkt);
    virtual int writePacket(CanPacket* pkt);
    
private:

    // utility functions
    int readPacketBlocking(CanPacket* pkt);
    int readPacketPolling(CanPacket* pkt);

    // Returns 0 if copied a packet, 1 if the buffer was empty
    int getPacket(CanPacket* pkt);
    /*!
     * Reads from USB buffer into internal buffer if the USB buffer is not empty.
     * Then reads from the internal buffer into the CAN packet queue.
     *
     * returns: number of CAN packets put into queue.
     */
    int tryToRead();

    // handle to the USB device inside our Segway
    FT_HANDLE ftHandle_;

    // event handle we'll use to block on data arrivale
    EVENT_HANDLE eventHandle_;
    
    // a buffer to read USB stream into
    std::vector<unsigned char> charBuffer_;
    
    unsigned int residualBytes_;
    
    // a buffer to store converted CAN messages
    std::queue<CanPacket> canBuffer_;
    
    int resetDevice();
    
    int readFromUsbToBuffer();
    int readFromUsbToBufferBlocking();
    
    int readFromBufferToQueue( int bytesInBuffer );
    
    int parseUsbToCan( CanPacket *pkt, unsigned char *bytes );
    
    int parseCanToUsb( CanPacket *pkt, unsigned char *bytes );
    
    unsigned char usbMessageChecksum( unsigned char *msg );    
};

#endif
