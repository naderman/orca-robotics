/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <assert.h>
#include <hydroutil/mathdefs.h>

#include "sickutil.h"

using namespace std;

namespace sickutil {

bool 
isClose( float val1, float val2 )
{
    return ( fabsf(val1-val2) < (float)1e-5 );
}

bool 
isClose( double val1, double val2 )
{
    return ( fabs(val1-val2) < 1e-5 );
}

bool 
isConfigValid( double rangeResolution, double angleIncrement, int baudrate )
{
    if ( (!isClose(rangeResolution, 0.001)) && (!isClose(rangeResolution, 0.01)) )
    {
        cout<<"bad range resolution." << endl;
        return false;
    }

    if ( !isClose(angleIncrement,DEG2RAD(0.5)) && !isClose(angleIncrement,DEG2RAD(1.0)) )
    {
        cout<<"bad angle." << endl;
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

double 
rangeResolution2MaxRange( double rangeResolution )
{
    if ( isClose( rangeResolution, 0.001 ) )
        return 8.0;
    else if ( isClose( rangeResolution, 0.01 ) )
        return 80.0;
    else
    {
        assert( false && "Didn't expect this range resolution." );
        return 0.0;
    }
}

double 
maxRange2RangeResolution( double maxRange )
{
    if ( isClose( maxRange, 8.0 ) )
        return 0.001;
    else if ( isClose( maxRange, 80.0 ) )
        return 0.01;
    else
    {
        assert( false && "Didn't expect this max range." );
        return 0.0;
    }
}

double 
numberOfSamples2AngleIncrement( int numberOfSamples )
{
    if ( numberOfSamples == 181 )
        return DEG2RAD(1.0);
    else if ( numberOfSamples == 361 )
        return DEG2RAD(0.5);
    else
    {
        assert( false && "Didn't expect this number of returns." );
        return 0.0;
    }
}

int 
angleIncrement2NumberOfSamples( double angleIncrement )
{
    if ( isClose( angleIncrement, 1.0 ) )
        return 181;
    else if ( isClose( angleIncrement, 0.5 ) )
        return 361;
    else
    {
        assert( false && "Didn't expect this angle increment." );
        return 0;
    }
}

} // namespace
