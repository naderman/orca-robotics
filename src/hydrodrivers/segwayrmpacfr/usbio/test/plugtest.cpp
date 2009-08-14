#include <iostream>
#include <ftd2xx.h>

using namespace std;

#define SEGWAY_USB_VENDOR_ID                    0x0403
#define SEGWAY_USB_PRODUCT_ID                   0xE729
#define SEGWAY_USB_DESCRIPTION                  "Robotic Mobile Platform"

void open( FT_HANDLE &ftHandle_ )
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
        cout<<"FT_OpenEx failed ("<<ftStatus<<")"<<endl;
        exit(1);
    }
    cout<<"FT_OpenEx OK"<<endl;
}

void close( FT_HANDLE &ftHandle_ )
{
    FT_STATUS ftStatus = FT_Close( ftHandle_ );
    if ( ftStatus != FT_OK )
        cout << "ERROR(usbftdi.cpp): FT_Close failed: " << ftStatus << endl;
    else
        cout<<"TRACE(plugtest.cpp): FT_Close worked." << endl;
}

int main()
{
    FT_HANDLE ftHandle_;

    open( ftHandle_ );

    cout<<"Un-plug then re-plug the USB, then hit return." << endl;

    getchar();

    close( ftHandle_ );
    open( ftHandle_ );

    cout<<"TRACE(segtest.cpp): clean exit." << endl;
    return 0;
}
