#include "usbftdi.h"
#include <iostream>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <sstream>

using namespace std;

namespace usbftdi {

UsbFtdi::UsbFtdi( DWORD iVID, DWORD iPID, const std::string &description, int debugLevel )
{
    // without this, the FTDI library will not find the device
    FT_SetVIDPID(iVID, iPID);   // use our VID and PID;

    FT_STATUS ftStatus;
    
    char desc[64];
    strcpy(desc,description.c_str());
    ftStatus = FT_OpenEx(desc, FT_OPEN_BY_DESCRIPTION, &ftHandle_ );
    if ( ftStatus != FT_OK) {
        throw Exception( ERROR_INFO, "FT_OpenEx failed", ftStatus );
    }
    if ( debugLevel_ > 4 )
        cout<<"FT_OpenEx OK"<<endl;

    ftStatus = FT_SetBaudRate( ftHandle_, 460800 );
    if ( ftStatus != FT_OK)  {
        throw Exception( ERROR_INFO, "FT_SetBaudRate failed", ftStatus );
    }   
    if ( debugLevel_ > 4 )
        cout<<"FT_SetBaudRate OK"<<endl;

    //set the latency timer to 2ms (valid range is from 2 to 255 ms)
    ftStatus = FT_SetLatencyTimer( ftHandle_, 2 );
    if ( ftStatus != FT_OK)  {
        throw Exception( ERROR_INFO, "FT_SetLatencyTimer failed", ftStatus );
    }   
    if ( debugLevel_ > 4 )
        cout<<"FT_SetLatencyTimer OK"<<endl;

    // debug
    /*
    CanPacket ppp;
    for ( int i=0; i<10; ++i )
    {
        ReadPacket( &ppp, 0 );
            IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(50));
    }
    */
    
    reset();

    //
    // Since we got to here, we found the USB device OK.  Initialise stuff we'll need
    //
    pthread_mutex_init(&(eventHandle_.eMutex), NULL);
    pthread_cond_init(&(eventHandle_.eCondVar), NULL);
}

UsbFtdi::~UsbFtdi()
{
    if ( debugLevel_ > 1 )
        cout<<"TRACE(usbftdi.cpp): destructor()" << endl;

    assert( ftHandle_ );

    // Nothing we can do about errors here...
    cout<<"TRACE(usbftdi.cpp): ============ Calling FT_Close =========" << endl;
    FT_STATUS ftStatus = FT_Close( ftHandle_ );
    if ( ftStatus != FT_OK )
        cout << "ERROR(usbftdi.cpp): FT_Close failed: " << ftStatusToString(ftStatus) << endl;

#if 0
    int ret;
    cout<<"TRACE(usbftdi.cpp): pthread_mutex_destroy" << endl;
    ret = pthread_mutex_destroy( &eventHandle_.eMutex );
    if ( ret != 0 )
    {
        cout << "ERROR(usbftdi.cpp): Error destroying mutex" << endl;
    }

    // AlexB: For some reason, this line was sometimes hanging...
    //        Perhaps the FTDI lib destroys it in FT_Close??
    //
    cout<<"TRACE(usbftdi.cpp): pthread_cond_destroy" << endl;
    ret = pthread_cond_destroy( &eventHandle_.eCondVar );
    if ( ret != 0 )
    {
        cout << "ERROR(usbftdi.cpp): Error destroying condition variable" << endl;
    }
#endif

    if ( debugLevel_ > 1 )
        cout<<"TRACE(usbftdi.cpp): destructor() done." << endl;
}

void
UsbFtdi::reset()
{
    assert( ftHandle_ );

    FT_STATUS ftStatus;

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "FT_ResetDevice failed", ftStatus );
    }

    ftStatus = FT_Purge( ftHandle_, FT_PURGE_RX | FT_PURGE_TX);
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "FT_Purge failed", ftStatus );
    }

    ftStatus = FT_ResetDevice( ftHandle_ );
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "FT_ResetDevice failed", ftStatus );
    }
    /*
        //extend timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 1000, 1000 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        cout<<"FT_SetTimeouts failed ("<<ftStatusToString(ftStatus)<<")"<<endl;
    } else {
        cout<<"FT_SetTimeouts OK"<<endl;
    }
    */
    //IceUtil::ThreadControl::sleep(IceUtil::Time::microSeconds(150));
    usleep(150000);

    //normal timeout while board finishes reset
    ftStatus = FT_SetTimeouts( ftHandle_, 0, 0 );  // timeouts in ms
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "FT_SetTimeouts failed", ftStatus );
    }
}

void 
UsbFtdi::printQueueStatus()
{
    // debug
    DWORD bytesInRxUsb, bytesInTxUsb, usbEvent;
    FT_STATUS ftStatus = FT_GetStatus( ftHandle_, &bytesInRxUsb, &bytesInTxUsb, &usbEvent );
    if ( ftStatus == FT_OK )  {
        cout<<"  UsbFtdi: Status: rx: "<<bytesInRxUsb<<" tx: "<<bytesInTxUsb<<" ev: "<<usbEvent<<endl;
    }
    else {
        cout<<"FT_GetStatus failed: "<<ftStatusToString(ftStatus)<<endl;
    }
}

void 
UsbFtdi::write( const unsigned char *data, int numBytes )
{
    // Deal with non-const-correct FTDI library
    unsigned char* nonConstData = const_cast<unsigned char*>(data);

    DWORD bytesWritten;
    FT_STATUS ftStatus = FT_Write( ftHandle_, nonConstData, numBytes, &bytesWritten );
    if ( ftStatus != FT_OK )
    {
        if ( debugLevel_ > 0 )
        {
            cout << "ERROR(usbftdi.cpp): After FT_Write failure: " << endl;
            printQueueStatus();
        }
        throw Exception( ERROR_INFO, "FT_Write failed", ftStatus );
    }
}

int
UsbFtdi::waitForData()
{
    DWORD EventMask = FT_EVENT_RXCHAR;
    FT_STATUS ftStatus = FT_SetEventNotification( ftHandle_, EventMask, (PVOID)&eventHandle_ );
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "FT_SetEventNotification failed", ftStatus );
    }

    // we want to timeout from listening to USB after 100ms (10Hz)
    // this is reasonable because, normally the packets comes in at >100Hz.
    int dusec = 100000; // [usec]
    // ugly-ass C time structures
    timeval tval;
    gettimeofday( &tval, NULL );
    if ( (tval.tv_usec + dusec)<1000000 ) {
        tval.tv_usec = tval.tv_usec + dusec;
    }
    else {
        ++tval.tv_sec;
        tval.tv_usec = tval.tv_usec + dusec - 1000000;
    }
    timespec abstime;
    abstime.tv_sec = tval.tv_sec;
    abstime.tv_nsec = tval.tv_usec*1000;

    pthread_mutex_lock(&eventHandle_.eMutex);
    //pthread_cond_wait(&eventHandle_.eCondVar, &eventHandle_.eMutex);
    int ret = pthread_cond_timedwait(&eventHandle_.eCondVar, &eventHandle_.eMutex, &abstime );
    pthread_mutex_unlock(&eventHandle_.eMutex);

    // this should not happen
    assert( (ret==0 || ret==ETIMEDOUT) && "pthread error in UsbFtdi::readBlocking" );

    if ( ret == 0 )
    {
        //cout<<"TRACE(usbftdi.cpp): Event was triggered." << endl;
        //printQueueStatus();
        return USBFTDI_OK;
    }
    else if ( ret == ETIMEDOUT ) {
        return USBFTDI_TIMEOUT;
    }
    else {
        std::stringstream ss;
        ss << "Error returned from pthread_cond_timedwait: " << strerror(ret);
        throw Exception( ERROR_INFO, ss.str() );
    }
}

int 
UsbFtdi::bytesInRxQueue()
{
    FT_STATUS   ftStatus;    
    DWORD       bytesInRxUsb;

    // Query status to find out how many bytes are in the receive buffer 
    ftStatus = FT_GetQueueStatus( ftHandle_, &bytesInRxUsb );
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "bytesInRxQueue(): FT_GetQueueStatus failed", ftStatus );
    }
    return bytesInRxUsb;
}

int
UsbFtdi::readNonBlocking( unsigned char *bytes, int maxBytesToRead )
{
    int bytesInRxUsb = bytesInRxQueue();

    // If there's nothing in the receive buffer, then just return
    if ( bytesInRxUsb == 0) {
        return 0;
    }

    //
    // Read from the USB buffer
    //
    FT_STATUS  ftStatus;    
    DWORD      numBytesRead;
    ftStatus = FT_Read( ftHandle_, bytes, maxBytesToRead, &numBytesRead );
    if ( ftStatus != FT_OK ) {
        throw Exception( ERROR_INFO, "readNonBlocking(): FT_Read failed", ftStatus );
    }
    return numBytesRead;
}

}
