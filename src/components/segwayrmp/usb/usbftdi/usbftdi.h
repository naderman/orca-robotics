#ifndef USBFTDI_H
#define USBFTDI_H

#include <ftd2xx.h>
#include <string>
#include "exceptions.h"

namespace usbftdi
{

    // Return codes
    const int USBFTDI_OK=0;
    const int USBFTDI_TIMEOUT=-1;


//!
//! @author Alex Brooks
//!
class UsbFtdi
{

public: 

    // 
    // Params are:
    //   - Vendor ID
    //   - Product ID
    //   - Description (as the device reports itself)
    //
    UsbFtdi( DWORD iVID, DWORD iPID, const std::string &description, int debugLevel=0 );
    ~UsbFtdi();

    void write( const unsigned char *data, int numBytes );

    // 
    // Waits for one or more bytes to arrive
    //
    // Timeout is 100ms (TODO: this should be configurable)
    //
    // Returns: 
    //   USBFTDI_OK
    //   USBFTDI_TIMEOUT 
    int waitForData();

    int bytesInRxQueue();

    // Returns the number of bytes read.
    // Caller has the responsibility to ensure that there's enough space.
    int readNonBlocking( unsigned char *bytes, int maxBytesToRead );

private: 

    void reset();

    // for debugging
    void printQueueStatus();

    // handle to the USB device
    FT_HANDLE ftHandle_;

    // event handle we'll use to block on data arrival
    EVENT_HANDLE eventHandle_;

    int debugLevel_;
};

}

#endif
