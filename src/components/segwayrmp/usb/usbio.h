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

#ifndef ORCA2_SEGWAY_RMP_USB_IO_H
#define ORCA2_SEGWAY_RMP_USB_IO_H

class CanPacket;

// This is hardware interface class for RMP robots with USB connectors.
// Because internally RMP communicates over CAN bus, this IO interface
// reads and writes CAN (!) packets.
//
// Does not throw any exceptions.
class UsbIo
{
public:

    enum UsbIoStatus
    {
        OK              = 0,
        NO_DATA         = 1,
        IO_ERROR        = -1,
        OTHER_ERROR     = -2
    };

    virtual ~UsbIo() {};

    /*
     * Initializes the USB device.
     * Returns: OK on success.
     */
    virtual UsbIoStatus init()=0;

    // Tries to reset the device whitout shutting it down completely.
    virtual UsbIoStatus reset()=0;
    
    /*
     * Closes the USB device
     * Returns: OK on success.
     */
    virtual UsbIoStatus shutdown()=0;

    /*
     * Blocks until a packet is available.
     * Returns OK if copied a packet, NO_DATA if not, or a negative ERROR code.
     */
    virtual UsbIoStatus readPacket(CanPacket* pkt)=0;
    
    /*
     * Returns OK on success, or a negative ERROR code otherwise.
     */
    virtual UsbIoStatus writePacket(CanPacket* pkt)=0;
    
};

#endif
