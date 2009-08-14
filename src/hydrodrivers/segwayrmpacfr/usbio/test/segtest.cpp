#include <iostream>

#include "../usbftdiutil.h"
#include "../rmpusbdataframe.h"
#include "../rmpdefs.h"
#include "../canpacket.h"
#include <IceUtil/IceUtil.h>

// magic numbers
#define SEGWAY_USB_VENDOR_ID                    0x0403
#define SEGWAY_USB_PRODUCT_ID                   0xE729
#define SEGWAY_USB_VENDOR_PRODUCT_ID            0x0403E729
#define SEGWAY_USB_DESCRIPTION                  "Robotic Mobile Platform"
#define SEGWAY_USB_MESSAGE_SIZE                 18
#define SEGWAY_USB_MSG_START                    0xF0
#define SEGWAY_USB_HEARTBEAT_MESSAGE            0x0
#define SEGWAY_USB_STATUS_MESSAGE               0x55
#define SEGWAY_USB_CHANNEL_A                    0xAA
#define SEGWAY_USB_CHANNEL_B                    0xBB

using namespace std;
using namespace segwayrmpacfr;

namespace {
    const int lastTrans_ = 0;
    const int lastRot_   = 0;
}


void init( FT_HANDLE &ftHandle_ )
{
    // custom device settings
    DWORD iVID = SEGWAY_USB_VENDOR_ID;
    DWORD iPID = SEGWAY_USB_PRODUCT_ID;
    // without this, the FTDI library will not find the segway
    FT_SetVIDPID(iVID, iPID);   // use our VID and PID;

    FT_STATUS ftStatus;

        char Description[64];
    strcpy(Description,SEGWAY_USB_DESCRIPTION);
    ftStatus = FT_OpenEx(Description ,FT_OPEN_BY_DESCRIPTION, &ftHandle_ );
    if ( ftStatus != FT_OK) {
        cout<<"FT_OpenEx failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
        exit(1);
    }
    cout<<"FT_OpenEx OK"<<endl;

    ftStatus = FT_SetBaudRate( ftHandle_, 460800 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetBaudRate failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
        FT_Close( ftHandle_ );        //close open device
        exit(1);
    }   
    cout<<"FT_SetBaudRate OK"<<endl;

    //set the latency timer to 2ms (valid range is from 2 to 255 ms)
    ftStatus = FT_SetLatencyTimer( ftHandle_, 2 );
    if ( ftStatus != FT_OK)  {
        cout<<"FT_SetLatencyTimer failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
        //latency not set - but we won't know   return -82;
        exit(1);
    }    
    cout<<"FT_SetLatencyTimer OK"<<endl;
}


void reset( FT_HANDLE &ftHandle_ )
{
    assert( ftHandle_ );

    FT_STATUS ftStatus;

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }

    ftStatus = FT_Purge( ftHandle_, FT_PURGE_RX | FT_PURGE_TX);
    if ( ftStatus != FT_OK ) {
        cout<<"FT_Purge failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
    } else {
        cout<<"FT_Purge OK"<<endl;
    }

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        cout<<"FT_ResetDevice failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
    } else {
        cout<<"FT_ResetDevice OK"<<endl;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(150));

    //normal timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 0, 0 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }

    if ( ftStatus == FT_OK ) {
        // return UsbIo::OK;
    }
    else {
        exit(1);
    }
}

unsigned char
usbMessageChecksum( unsigned char *msg )
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

void parseCanToUsb( CanPacket *pkt, unsigned char *bytes )
{
    bytes[0] = SEGWAY_USB_MSG_START;   //SOH (alexm: start of header?)
    bytes[1] = SEGWAY_USB_STATUS_MESSAGE;
    bytes[2] = 0;   // CAN A
    bytes[3] = 0;   // always 0
    bytes[4] = 0;   // always 0
    bytes[5] = 0;   // always 0
    bytes[6] = pkt->id() >> 8;
    bytes[7] = pkt->id() ;
    bytes[8] = 0;   // always 0

    for ( int i = 0; i < 8; ++i )
    {
        bytes[9+i] = pkt->msg[i];
    }
    
    // do check sum
    bytes[17] = usbMessageChecksum( bytes );
}

void
makeStatusCommandPacket( CanPacket* pkt, uint16_t commandId, uint16_t value )
{
    pkt->setId( RMP_CAN_ID_COMMAND );

    // put last motion command into the packet
    pkt->putSlot(0, (uint16_t)lastTrans_);
    pkt->putSlot(1, (uint16_t)lastRot_);
    pkt->putSlot(2, commandId);
    pkt->putSlot(3, value);
}

void tryReOpen( FT_HANDLE &ftHandle_ )
{
    if ( FT_Close( ftHandle_ ) == FT_OK ) {
        init( ftHandle_ );
        reset( ftHandle_ );
        cout<<"TRACE(segtest.cpp): Re-opened port." << endl;
    }
    else {
        cout<<"TRACE(segtest.cpp): tryReOpen failed..." << endl;
    }
}

void write( FT_HANDLE &ftHandle_, unsigned char *bytes, int length )
{
    DWORD bytesWritten;
    FT_STATUS ftStatus = FT_Write( ftHandle_, bytes, length, &bytesWritten );
    if ( ftStatus != FT_OK ) {
        cout<<"write(): FT_Write failed: result: "<<ftStatusToString(ftStatus)
            <<", bytesWritten: "<<bytesWritten<<endl;

        DWORD bytesInRxUsb, bytesInTxUsb, usbEvent;
        FT_STATUS ftStatusA = FT_GetStatus( ftHandle_, &bytesInRxUsb, &bytesInTxUsb, &usbEvent );
        if ( ftStatusA == FT_OK )  {
            cout<<"Status after write: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
        }
        else {
            cout<<"FT_GetStatus failed: "<<ftStatusToString(ftStatusA)<<endl;
        }

        tryReOpen( ftHandle_ );
        // exit(1);
    }
    else {
        cout<<"TRACE(segtest.cpp): write succeeded." << endl;
    }
}

int main()
{
    FT_HANDLE ftHandle_;

    init( ftHandle_ );
    reset( ftHandle_ );

//     init( ftHandle_ );
//     reset( ftHandle_ );

    CanPacket *pkt_ = new CanPacket;
    unsigned char bytes[SEGWAY_USB_MESSAGE_SIZE];
    makeStatusCommandPacket( pkt_, RMP_CMD_RESET_INTEGRATORS, RMP_CAN_RESET_ALL );
    // convet CAN packet to USB message
    parseCanToUsb( pkt_, bytes );

    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );
    write( ftHandle_, bytes, SEGWAY_USB_MESSAGE_SIZE );

    cout<<"TRACE(segtest.cpp): clean exit." << endl;
    return 0;
}
