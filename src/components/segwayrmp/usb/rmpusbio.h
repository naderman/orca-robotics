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

// This is hardware interface class for RMP robots with USB connectors.
// Because internally RMP communicates over CAN bus, this IO interface
// reads and writes CAN (!) packets.
//
class RmpUsbIo
{
public:

    enum RmpUsbIoStatus
    {
        OK              = 0,
        NO_DATA         = 1,
    };

    virtual ~RmpUsbIo() {};

    /*
     * Initializes the USB device.
     */
    virtual void init()=0;

    // Tries to reset the device without shutting it down completely.
    virtual void reset()=0;
    
    /*
     * Closes the USB device
     */
    virtual void shutdown()=0;

    /*
     * Blocks until a packet is available.
     * Returns OK if copied a packet, NO_DATA if not
     */
    virtual RmpUsbIoStatus readPacket(CanPacket* pkt)=0;
    
    /*
     */
    virtual void writePacket(CanPacket* pkt)=0;
    
};

} // namespace

#endif
