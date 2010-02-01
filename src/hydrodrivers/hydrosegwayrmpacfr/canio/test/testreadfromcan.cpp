#include <iostream>
#include <libpcan.h>
#include <pcan.h>
#include <fcntl.h> // for O_RDWR
#include <sstream>
#include <string.h>
#include <gbxutilacfr/exceptions.h>
#include "../peakutil.h"
#include <iomanip>

using namespace std;

int main()
{
    const std::string portName = "/dev/pcan40";
    HANDLE portHandle = LINUX_CAN_Open(portName.c_str(), O_RDWR); 
    if ( portHandle == NULL )
    {
        stringstream ss;
        ss << "CanPeakRmpIo::constructor(): Error: "<<
            "Unable to open the can port '" << portName << "': "<<strerror(nGetLastError());
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    cout<<"TRACE(testcan.cpp): version: " << segwayrmpacfr::versionString(portHandle) << endl;

    DWORD retVal = CAN_Init(portHandle, CAN_BAUD_500K, CAN_INIT_TYPE_EX );
    
    if( retVal != 0 ){
        stringstream ss;
        ss << endl << " --> Attempted initialisation of the CAN card failed: " << segwayrmpacfr::errorInfo(retVal) << endl;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );      
    }

    while ( true )
    {
        try {
            int numOK = 0;
//    for ( size_t i=0; i < 1000; i++ )
            while ( true )
            {
                int nMicroSeconds = 12*1000;
                TPCANRdMsg msg;
                DWORD ret = LINUX_CAN_Read_Timeout( portHandle, &msg, nMicroSeconds );
                if ( ret == 0 )
                {
//                    cout<<"TRACE(testcan.cpp): read message: " << segwayrmpacfr::toString(msg.Msg) << endl;
                    numOK++;
                    if ( numOK==1 )
                        cout<<"TRACE(testcan.cpp): read message: " << segwayrmpacfr::toString(msg.Msg) << endl;
                    if ( msg.Msg.ID==0 )
                    {
                        cout<<"TRACE(testcan.cpp): read message with ID 0x00: " << segwayrmpacfr::toString(msg.Msg) << endl;
                        return 1;
                    }
                }
                else
                {
                    stringstream ss;
                    ss << "LINUX_CAN_Read_Timeout failed.  "<<segwayrmpacfr::errorInfo(ret);
                    throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
                }
            }
        }
        catch ( std::exception &e )
        {
            cout<<"TRACE(testcan.cpp): " << e.what() << endl;
        }
    }

    return 0;
}
