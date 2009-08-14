#include <iostream>
#include <assert.h>
#include <cmath>

#define INTERESTING_YAW_RATE_DEG_SEC -68

using namespace std;

//
// Simple utility for parsing CAN logfiles
//

const char *USAGE_ARGS = "<log_filename>";

int16_t slotVal( int hi, int lo )
{
    int val = hi;
    val = val << 8;
    val |= lo;
    return val;
}

int main( int argc, char **argv )
{
    if ( argc < 2 )
    {
        cout << "USAGE: " << argv[0] << " " << USAGE_ARGS << endl;
        exit(1);
    }

    FILE *f = fopen( argv[1], "r" );
    assert( f );

    size_t size=0;
    char *line;
    int ret;
    double prevIntYawDegs = 0;

    double yawRateSum=0;
    int    yawRateNum=0;
    double intYawSum=0;
    int    intYawNum=0;

    while ( true )
    {
        ret = getline( &line, &size, f );
        if ( ret == -1 ) break;

        int sec,usec,id,d1,d2,d3,d4,d5,d6,d7,d8;
        ret = sscanf( line, "%d:%d id_:%x %x %x %x %x %x %x %x %x", 
                      &sec,&usec,&id,&d1,&d2,&d3,&d4,&d5,&d6,&d7,&d8 );
        assert( ret == 11 );

        if ( id == 0x0407 )
        {
            // Command
            // double commandYawRateDegSec = slotVal(d3,d4)/(1024.0/180.0);
            int yawRateCounts = slotVal(d3,d4);
            int speedCounts = slotVal(d1,d2);
            double commandYawRateDegSec = yawRateCounts/(1120.0/360.0);
            double speed = speedCounts/(332.0);
            cout << "Cmd : id="<<hex<<id<<dec<<", speed("<<speedCounts<<") = "<<speed<<"m/s, yawRate("<<yawRateCounts<<") = "<<commandYawRateDegSec<<" deg/s"<<endl;
        }
        else if ( id == 0x0402 )
        {
            // Rates
            int yawRateCounts = slotVal(d5,d6);
            double yawRateDegSec = -yawRateCounts/7.8;
            cout << "Rate: id="<<hex<<id<<dec<<", yawRate("<<yawRateCounts<<") = "<<yawRateDegSec<<" deg/s"<<endl;
            if ( fabs(yawRateDegSec-INTERESTING_YAW_RATE_DEG_SEC) < 10 )
            {
                yawRateNum++;
                yawRateSum+=yawRateDegSec;
            }
        }
        else if ( id == 0x0404 )
        {
            // Integrated values
            int16_t intYawLo = slotVal(d5,d6);
            int16_t intYawHi = slotVal(d7,d8);
            int intYaw32 = (intYawHi<<16) | intYawLo;
            double intYawDegs = intYaw32/112644.0 * 360.0;

            const double period = 0.01;
            double rate = (intYawDegs-prevIntYawDegs) / period;

            cout << "Int : id="<<hex<<id<<dec<<", yaw = "<<intYawDegs<<" deg, rate="<<rate<<" deg/s"<<endl;
            prevIntYawDegs = intYawDegs;

            if ( fabs(rate-INTERESTING_YAW_RATE_DEG_SEC) < 10 )
            {
                intYawNum++;
                intYawSum+=rate;
            }
        }
    }

    cout<<"avg yawRateDegSec: " << yawRateSum/yawRateNum << endl;
    cout<<"avg intYawRate:    " << intYawSum/intYawNum << endl;

    fclose(f);
    return 0;
}
