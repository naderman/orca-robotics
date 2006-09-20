/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_USB_IO_H
#define ORCA2_SEGWAY_RMP_USB_IO_H

namespace segwayrmp
{

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

} // namespace

#endif
