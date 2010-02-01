#include <iostream>
#include "../canpeakrmpio.h"
#include <gbxutilacfr/exceptions.h>

using namespace std;
using namespace segwayrmpacfr;

//
// Creates canpeakrmpio and reads packets
//
int main()
{
    CanPeakRmpIo rmp( "/dev/pcan40" );
    const int debugLevel = 0;

    while ( true )
    {
        try {
            rmp.enable( debugLevel );
            CanPacket pkt;
            while ( true )
            {
                RmpIo::RmpIoStatus status = rmp.readPacket( pkt );
                if ( status != RmpIo::OK )
                {
                    throw gbxutilacfr::Exception( ERROR_INFO, "Bad read status" );
                }
            }
        }
        catch ( std::exception &e )
        {
            cout<<"TRACE(rmpsimpletest.cpp): " << e.what() << endl;
        }
    }

    return 0;
}
