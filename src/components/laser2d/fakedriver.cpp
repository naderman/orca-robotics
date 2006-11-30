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

FakeDriver::FakeDriver( const Config & cfg, const orcaice::Context & context )
    : Driver(cfg),
      context_(context)
{
}

FakeDriver::~FakeDriver()
{
}

int
FakeDriver::init()
{ 
    return 0;
}

int 
FakeDriver::read( orca::LaserScanner2dDataPtr &data )
{
    context_.tracer()->info( "Generating fake laser data..." );

    orcaice::setToNow( data->timeStamp );
    
    data->minRange          = config_.minRange;
    data->maxRange          = config_.maxRange;
    data->fieldOfView       = config_.fieldOfView;
    data->startAngle        = config_.startAngle;

    data->ranges.resize(config_.numberOfSamples);
    data->intensities.resize(config_.numberOfSamples);

    for ( unsigned int i=0; i < data->ranges.size(); i++ )
    {
        data->ranges[i] = (float)((i/(float)(config_.numberOfSamples))*data->maxRange);
        data->intensities[i] = i%2;
    }

    usleep(100000);
    //IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}


} // namespace
