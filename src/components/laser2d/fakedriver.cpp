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
#include <orcaice/orcaice.h>

#include "fakedriver.h"

using namespace std;

namespace laser2d {

FakeDriver::FakeDriver( const orcaice::Context & context )
    : context_(context)
{
    // setting factory config parameters
    currentConfig_.maxRange         = 99.0;
    currentConfig_.fieldOfView      = 300.0;
    currentConfig_.startAngle       = -150.0;
    currentConfig_.numberOfSamples  = 100;
}

FakeDriver::~FakeDriver()
{
    disable();
}

int 
FakeDriver::read( orca::LaserScanner2dDataPtr &data )
{
    context_.tracer()->debug( "Generating fake laser data...", 6 );

    orcaice::setToNow( data->timeStamp );
    
    data->maxRange          = currentConfig_.maxRange;
    data->fieldOfView       = currentConfig_.fieldOfView;
    data->startAngle        = currentConfig_.startAngle;

    data->ranges.resize(currentConfig_.numberOfSamples);
    data->intensities.resize(currentConfig_.numberOfSamples);

    for ( unsigned int i=0; i < data->ranges.size(); i++ )
    {
        data->ranges[i] = (float)((i/(float)(currentConfig_.numberOfSamples))*data->maxRange);
        data->intensities[i] = i%2;
    }

    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeDriver::getConfig( Config &cfg )
{
    cfg = currentConfig_;
    return 0;
}

int 
FakeDriver::setConfig( const Config &cfg )
{
    currentConfig_ = cfg;
    
    return 0;
}

} // namespace
