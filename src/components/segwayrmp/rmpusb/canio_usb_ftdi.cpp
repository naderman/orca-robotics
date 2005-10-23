/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "canio_usb_ftdi.h"

#include "rmpusbdataframe.h"

#include <iostream>
#include <assert.h>
#include <orcaiceutil/orcadebug.h>

// magic numbers
#define SEGWAY_USB_VENDOR_ID                    0x0403
#define SEGWAY_USB_PRODUCT_ID                   0xE729
#define SEGWAY_USB_MESSAGE_SIZE                 18
#define SEGWAY_USB_MESSAGE_START_OF_HEADER      0xF0
#define SEGWAY_USB_HEARTBEAT_MESSAGE            0x0
#define SEGWAY_USB_STATUS_MESSAGE               0x55
#define SEGWAY_USB_CHANNEL_A                    0xAA
#define SEGWAY_USB_CHANNEL_B                    0xBB

using namespace std;


CanioUsbFtdi::CanioUsbFtdi() :
    debugMsgCount_(0)
{
    // Set initial buffer size
    charBuffer_.resize(128);

    residualBytes = 0;
}

CanioUsbFtdi::~CanioUsbFtdi()
{
}

/* Initializes the class by opening 2 can channels at the given
 * frequency, which should be one of the #defined BAUD_*K values
 *
 * returns: 0 on success, negative on error
 */
int
CanioUsbFtdi::Init()
{
  cout<<"CanioUsbFtdi::Init: initizlizing device"<<endl;
  FT_STATUS ftStatus;
  DWORD num_devices;

  // custom device settings
  DWORD iVID = SEGWAY_USB_VENDOR_ID;
  DWORD iPID = SEGWAY_USB_PRODUCT_ID;
  // without this the library will not find the segway
  FT_SetVIDPID(iVID, iPID);   // use our VID and PID;

  ftStatus = FT_ListDevices( &num_devices, NULL, FT_LIST_NUMBER_ONLY );
  if ( ftStatus != FT_OK )
  {
      cout<<"couldn't get a list of devices"<<endl;
      return -1;
  }

  cout<<"found "<<(int)num_devices<<" devices"<<endl;
  if (num_devices <= 0) {
      return -1;
  }

  ftStatus = FT_Open( 0, &ftHandle_ );
  if ( ftStatus != FT_OK) {
      cout<<"FT_Open failed ("<<ftStatus<<")"<<endl;
  }
  else {
      cout<<"FT_Open OK"<<endl;

      ftStatus = FT_SetBaudRate( ftHandle_, 460800 );
      if ( ftStatus != FT_OK)  {
          cout<<"FT_SetBaudRate failed ("<<ftStatus<<")"<<endl;
          FT_Close( ftHandle_ );        //close open device
          return 0;
      }
      else {
          cout<<"FT_SetBaudRate OK"<<endl;
      }

        //set the latency timer to 2ms (valid range is from 2 to 255 ms)
      ftStatus = FT_SetLatencyTimer( ftHandle_, 2 );
      if ( ftStatus != FT_OK)  {
          cout<<"FT_SetLatencyTimer failed ("<<ftStatus<<")"<<endl;
            //latency not set - but we won't know   return -82;
      }
      else {
          cout<<"FT_SetLatencyTimer OK"<<endl;
      }

      ftStatus = this->resetDevice();

      // debug
      /*
      CanPacket ppp;
      for ( int i=0; i<10; ++i )
      {
          ReadPacket( &ppp, 0 );
          usleep(50);
  } */
  }

  return ftStatus;
}

int CanioUsbFtdi::resetDevice()
{
    assert( ftHandle_ );

    FT_STATUS ftStatus;

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }

    ftStatus = FT_Purge( ftHandle_, FT_PURGE_RX | FT_PURGE_TX);
    if ( ftStatus != FT_OK ) {
        cout<<"FT_Purge failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_Purge OK"<<endl;
    }

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }
    /*
        //extend timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 1000, 1000 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }
    */
    usleep( 150 );

      //normal timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 0, 0 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatus<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }

    return ftStatus;
}
  
/* Closes the CAN channels
 *
 * returns: 0 on success, negative otherwise
 */
int
CanioUsbFtdi::Shutdown()
{
    return FT_Close( ftHandle_ );
}
  
/* Writes the given packet out on both channels
 *
 * returns: 0 on success, negative error code otherwise
 */
int CanioUsbFtdi::WritePacket( CanPacket &pkt )
{
    FT_STATUS ftStatus;
    /*
    DWORD bytesInRxUsb, bytesInTxUsb, usbEvent;
    ftStatus = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );
    //ftStatus = FT_GetStatus( ftHandle_, &bytesInRxUsb, &bytesInTxUsb, &usbEvent );
    if ( ftStatus == FT_OK )  {
        cout<<"Status on write: rx: "<<bytesInRxUsb<<endl;
        //cout<<"Status on write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    */
    unsigned char bytes[SEGWAY_USB_MESSAGE_SIZE];

    // convet CAN packet to USB message
    parseCanToUsb( &pkt, bytes );

    DWORD bytesWritten;
    ftStatus = FT_Write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE, &bytesWritten );
    if ( ftStatus == FT_OK ) {
        //DTRACE<<"wrote "<<bytesWritten<<" bytes"<<endl;
    } else {
        //DTRACE<<"ERROR: failed to write to USB"<<endl;  
    }

    return ftStatus;
}

/*!
 *  caluate the checksum for the message being sent.  Note that the buffer
 *  is expected to be exactly 18 bytes in length.
 */
unsigned char CanioUsbFtdi::usbMessageChecksum( unsigned char *msg )
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


int CanioUsbFtdi::GetNextPacket(CanPacket& pkt)
{

    if ( canBuffer_.size() > 0 )
    {
        pkt = canBuffer_.front();
        canBuffer_.pop();
        return 0;
    }
    else {
        return 1;
    }
}

/*
 * Reads from USB buffer into internal buffer if the USB buffer is not empty.
 * Reads from internal buffer into a CAN packet queue.
 *
 * returns: number of CAN packets put into queue.
 */
int CanioUsbFtdi::ReadPackets()
{
    int bytesInBuffer;
    int numPackets = 0;

    // Read from the USB buffer into our own buffer
    bytesInBuffer = readFromUsbToBuffer();

    // Process the buffer contents into CAN packets
    if( bytesInBuffer > 0 )
    {
        // Process the buffer contents into CAN packets
        numPackets = readFromBufferToQueue( bytesInBuffer );
    }

    return numPackets;
}

int CanioUsbFtdi::readFromUsbToBuffer()
{
    FT_STATUS   ftStatus;    
    DWORD       bytesInRxUsb;
    DWORD       bytesRead;
        
    // Query status to find out how many bytes are in the receive buffer 
    ftStatus = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );
    if ( ftStatus != FT_OK )
        bytesInRxUsb = 0;

    // If there's nothing in the receive buffer, then just return
    if ( bytesInRxUsb == 0)
        return 0;
    
    // Resize our buffer if necessary
    if( (bytesInRxUsb + residualBytes) > charBuffer_.size() )
        charBuffer_.resize( bytesInRxUsb + residualBytes );

    // Read from the USB buffer
    ftStatus = FT_Read( ftHandle_, &charBuffer_[residualBytes], bytesInRxUsb, &bytesRead );

    if ( ftStatus != FT_OK )
    {
        DTRACE << "read error" << endl;
        return 0;
    }

    // Check for read timeout
    if( bytesInRxUsb != bytesRead )
    {
        DTRACE << "READ TIMEOUT. bytesInRxUsb = " << bytesInRxUsb << "   bytesRead = " << bytesRead << endl;
    }

    // Return the number of bytes that are now in the buffer
    return residualBytes + bytesRead;
}

int CanioUsbFtdi::readFromBufferToQueue( int bytesInBuffer )
{
    int     pos             = 0;        // Buffer position indicator
    bool    done            = false;
    int     skippedBytes    = 0;
    int     numPackets      = 0;

    // While the buffer contains enough data for at least one message 
    while ( (bytesInBuffer - pos) >= SEGWAY_USB_MESSAGE_SIZE )
    {   
        bool    lookingForMsgStart  = true;

        // Scan buffer for the start of the next valid message
        while (lookingForMsgStart)
        {
            // If there aren't enough bytes left to contain a full message,
            // then kick out
            if( (bytesInBuffer - pos) < SEGWAY_USB_MESSAGE_SIZE )
            {
                done = true;
                break;
            }

            // Look for start-of-msg character at the start AND end of candidate message
            if( charBuffer_[pos] == SEGWAY_USB_MESSAGE_START_OF_HEADER )
            {        
                // Compare calculated checksum with value embedded in message. If
                // they agree, message found. Otherwise, keep looking. 
                if( (unsigned char)usbMessageChecksum(&charBuffer_[pos]) ==
                        charBuffer_[pos + (SEGWAY_USB_MESSAGE_SIZE - 1)])
                    lookingForMsgStart = false;
                else
                {
                    pos++;
                    skippedBytes++;
                }
            }
            else
            {
                pos++;
                skippedBytes++;
            }
        } // while (lookingForMsgStart)
        
        // Process message
        if ( !done )
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
                numPackets++;
                //printf("Stored packet: pkt: %s\n", pkt.toString());
            }

            // Increment position regardless of parsing success
            pos = pos + SEGWAY_USB_MESSAGE_SIZE;
        }
    } //  while ( (bytesInBuffer - (pos + 1)) >= SEGWAY_USB_MESSAGE_SIZE )

    // If there are any residual bytes in the buffer, move them to the beginning
    residualBytes = bytesInBuffer - pos;
    assert( residualBytes >= 0 );

    if( residualBytes > 0 )
        memcpy( &charBuffer_[0], &charBuffer_[pos], residualBytes );

    /*
    printf("%4i skipped bytes in this read cycle (%2.0f%% loss)\n",
        skippedBytes, ((float)skippedBytes/(float)bytesInBuffer)*100.0);
    */
    return numPackets;
}

// === LARGELY UNMODIFIED FROM SEGWAY WINDOWS DEMO SOFTWARE ===

/*!
 *  returns: # bytes in msg if a packet is valid and negative on error.
*/
int CanioUsbFtdi::parseUsbToCan( CanPacket *pkt, unsigned char *bytes )
{
    // do check sum: always fails
    
    //if ( bytes[17] != usbMessageChecksum( bytes ) ) {
        //DTRACE<<"error: checksum failed"<<endl;
    //    return -88;
    //}

    int ret;
    if (bytes[1] == SEGWAY_USB_HEARTBEAT_MESSAGE) {
        //this message is a HEARTBEAT
        ret = 0;
    }
    else if (bytes[1]==SEGWAY_USB_STATUS_MESSAGE && bytes[2]==SEGWAY_USB_CHANNEL_A )
    {            
        pkt->id = ((bytes[4] << 3) | ((bytes[5] >> 5) & 7)) & 0x0fff;

        if ( pkt->id>=RMP_CAN_ID_MSG1 && pkt->id<=RMP_CAN_ID_MSG8 )
        {
            for ( int i = 0; i < 8; ++i )
            {
                pkt->msg[i] = bytes[9+i];
            }
            ret = 8;
        }
        else {
            // this is not a status message, no need to queue it
            ret = 0;
        }
    }
    else {
        // we are f*d
        ret = -1;
    }

    return ret;
}

int CanioUsbFtdi::parseCanToUsb( CanPacket *pkt, unsigned char *bytes )
{
    bytes[0] = SEGWAY_USB_MESSAGE_START_OF_HEADER;   //SOH (alexm: start of header?)
    bytes[1] = SEGWAY_USB_STATUS_MESSAGE;
    bytes[2] = 0;  //CAN A
    bytes[3] = 0;
    bytes[4] = 0;
    bytes[5] = 0;
    bytes[6] = pkt->id >> 8;
    bytes[7] = pkt->id ;
    bytes[8] = 0;

    for ( int i = 0; i < 8; ++i )
    {
        bytes[9+i] = pkt->msg[i];
    }
    
    // do check sum
    bytes[17] = usbMessageChecksum( bytes );

    return 0;
}
