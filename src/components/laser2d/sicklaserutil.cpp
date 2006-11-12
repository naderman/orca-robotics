/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <assert.h>
#include <orcaobj/mathdefs.h>

#include "sicklaserutil.h"

using namespace std;

namespace sicklaserutil {

bool isClose( float val1, float val2 )
{
    return ( fabs(val1-val2) < 1e-5 );
}

// bool sickCanDoConfig( const RangeScanner2dConfigPtr &cfg, const int baudrate )
bool 
sickCanDoConfig( double rangeResolution, double angleIncrement, double maxRange, int baudrate )
{
    if ( (!isClose(rangeResolution, (float)0.001)) && (!isClose(angleIncrement, (float)0.01)) )
    {
        cout<<"bad range resolution." << endl;

        // bad rangeResolution
        return false;
    }
    if ( !isClose( maxRange, associatedMaxRange(rangeResolution) ) )
    {
        cout << "bad max range: "<<maxRange << endl;
        return false;
    }
    if ( (!isClose(angleIncrement, (float)(DEG2RAD(0.5)))) && (!isClose(angleIncrement, (float)(DEG2RAD(1.0)))) )
    {
        cout<<"bad angle." << endl;

        // bad angleIncrement
        return false;
    }
    switch (baudrate)
    {
        case 9600: break;
        case 19200: break;
        case 38400: break;
        case 500000: break;
        default:
            cout<<"bad baudrate." << endl;
            return false;
    }
    return true;

}

float associatedMaxRange( double rangeResolution )
{
    if ( isClose( rangeResolution, (float)0.001 ) )
        return 8.0;
    else if ( isClose( rangeResolution, (float)0.01 ) )
        return 80.0;
    else
    {
        assert( false && "Didn't expect this rangeResolution." );
        return 0.0;
    }
}

}
